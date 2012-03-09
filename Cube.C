#include <GL/glew.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <cmath>
#include <cstring>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"

#include "config.h"
#include "Cube.h"
#include "Shader.h"

using namespace std;

Cube::Cube()
  : _transformFeedback(0),
    _srcXfbBuff(0),
    _destXfbBuff(1),
    _particleGenVao(0),
    _indicesVbo(0),
    _vertsVbo(0),
    _vertQid(0),
    _vertCount(0),
    _genProg(),
    _surfaceProg(),
    _time(0.0f),
    _modelViewMatrix(1.0f),
    _projectionMatrix(1.0f)
{
  // Initialize OpenGL state
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glPointSize(2.0f);

  // Generate Cube geometry and store in _indices, _position3.
  static const float position[] =
    { -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f
    };
  static const unsigned index[] =
    { 0, 1, 2,
      0, 2, 3,
      1, 5, 6,
      1, 6, 2,
      5, 4, 7,
      5, 7, 6,
      4, 0, 3,
      4, 3, 7,
      3, 2, 6,
      3, 6, 7,
      1, 0, 4,
      1, 4, 5
    };
  _verts3.assign(position, position + sizeof(position) / sizeof(position[0]));
  _indices.assign(index, index + sizeof(index) / sizeof(index[0]));

  // Load particle generation shader.
  const GLchar *particleVaryings[] = {"gWorldPos", "gVelocity", "gAge"};
  _genProg.compile("Particle.Vertex", "Particle.Geometry", "Particle.Fragment");
  glTransformFeedbackVaryings(_genProg, 3, particleVaryings, GL_SEPARATE_ATTRIBS);
  _genProg.link();

  // Load particle velocity/decay/rendering shader.
  const GLchar *surfaceVaryings[] = {"gWorldPos", "gVelocity", "gAge"};
  _surfaceProg.compile("Surface.Vertex", "Surface.Geometry", "Surface.Fragment");
  glTransformFeedbackVaryings(_surfaceProg, 3, surfaceVaryings, GL_SEPARATE_ATTRIBS);
  _surfaceProg.link();

  // Create transform feedback objects.
  glGenTransformFeedbacks(1, &_transformFeedback);

  // Bind a vertex array object.
  // A VAO holds the state of all VBOs associated with it.
  glGenVertexArrays(1, &_particleGenVao);
  glBindVertexArray(_particleGenVao);
  
  // Create a VBO to hold per-vertex position data.  Point the
  //  "position" varying to this VBO.  (Note that _vao is still bound,
  //  so this VBO will be associated with the enabled VAO)
  GLuint posIdx = glGetAttribLocation(_genProg, "position");
  glGenBuffers(1, &_vertsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vertsVbo);
  glBufferData(GL_ARRAY_BUFFER, _verts3.size() * sizeof(GLfloat), &_verts3[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(posIdx);
  glVertexAttribPointer(posIdx, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Create a VBO to hold vertex index data. This VBO isn't attached
  // to a varying, since it simply provides an index into the other
  // VBOs at rendering time.  See glDrawElements().  (Note that _vao
  // is still bound, so this VBO will be associated with the enabled
  // VAO)
  glGenBuffers(1, &_indicesVbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);

  // Now that we're done uploading data to the OpenGL server, unbind
  // the VAO and VBO.
  glBindVertexArray(0);

  // Create vertex array and buffers for particle aging shader.
  glGenVertexArrays(2, _surfaceVao);
  glGenBuffers(2, _worldPosBo);
  glGenBuffers(2, _velocityBo);
  glGenBuffers(2, _ageBo);
  for (int i = 0; i < 2; ++i) {
    GLuint posIdx = glGetAttribLocation(_surfaceProg, "worldPos");
    GLuint velIdx = glGetAttribLocation(_surfaceProg, "velocity");
    GLuint ageIdx = glGetAttribLocation(_surfaceProg, "age");

    glBindVertexArray(_surfaceVao[i]);

    glEnableVertexAttribArray(posIdx);
    glBindBuffer(GL_ARRAY_BUFFER, _worldPosBo[i]);
    glBufferData(GL_ARRAY_BUFFER, 1000000 * 4 * sizeof(GLfloat), 0, GL_STREAM_COPY);
    glVertexAttribPointer(posIdx, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(velIdx);
    glBindBuffer(GL_ARRAY_BUFFER, _velocityBo[i]);
    glBufferData(GL_ARRAY_BUFFER, 1000000 * 3 * sizeof(GLfloat), 0, GL_STREAM_COPY);
    glVertexAttribPointer(velIdx, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(ageIdx);
    glBindBuffer(GL_ARRAY_BUFFER, _ageBo[i]);
    glBufferData(GL_ARRAY_BUFFER, 1000000 * 1 * sizeof(GLfloat), 0, GL_STREAM_COPY);
    glVertexAttribPointer(ageIdx, 1, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
  }

  // Generate query object IDs.
  glGenQueries(1, &_vertQid);

  // Populate the default modelview and projection matrices.
  _modelViewMatrix  = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -10.0));
  _projectionMatrix = glm::mat4(1.0f) * glm::perspective
    (25.0f, (float)RENDER_WIDTH /(float)RENDER_HEIGHT, 1.0f, 14.0f);

  return;
}

Cube::~Cube()
{
  // Delete all buffers and vao.
  glDeleteBuffers(1, &_vertsVbo);
  glDeleteVertexArrays(1, &_particleGenVao);

  // TODO delete transform feedback buffers and vao.
  // 

  // Delete transform feedback object.
  glDeleteTransformFeedbacks(1, &_transformFeedback);

  return;
}

void Cube::render(float secondsElapsed)
{
  GLuint mvpIdx = glGetUniformLocation(_genProg, "MVPMatrix");
  GLuint timIdx = glGetUniformLocation(_genProg, "Time");
  GLuint elaIdx = glGetUniformLocation(_genProg, "ElapsedSec");
  GLuint brtIdx = glGetUniformLocation(_genProg, "BirthFrequency");

  // Rotate.
  float degrees = secondsElapsed * 50.0f;
  _modelViewMatrix = glm::rotate(_modelViewMatrix, degrees, glm::vec3(0.3f, 1.0f, 0.1f));
  glm::mat4 mvpMatrix = _projectionMatrix * _modelViewMatrix;

  // Generate new points for this frame.
  glUseProgram(_genProg);
  glUniformMatrix4fv(mvpIdx, 1, false, &mvpMatrix[0][0]);
  glUniform1f(timIdx, _time);
  glUniform1f(elaIdx, secondsElapsed);
  glUniform1f(brtIdx, 1000.0f);
  
  glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, _vertQid);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _transformFeedback);
    glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
		      _worldPosBo[_srcXfbBuff],
		      4*_vertCount*sizeof(GLfloat),
		      4*_vertCount*sizeof(GLfloat) + 80*4*sizeof(GLfloat));
    glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 1,
		      _velocityBo[_srcXfbBuff],
		      3*_vertCount*sizeof(GLfloat),
		      3*_vertCount*sizeof(GLfloat) + 80*3*sizeof(GLfloat));
    glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 2,
		      _ageBo[_srcXfbBuff],
		      1*_vertCount*sizeof(GLfloat),
		      1*_vertCount*sizeof(GLfloat) + 80*1*sizeof(GLfloat));
    glBeginTransformFeedback(GL_POINTS);
    //    glEnable(GL_RASTERIZER_DISCARD);

    glBindVertexArray(_particleGenVao);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    glEndTransformFeedback();
    //    glDisable(GL_RASTERIZER_DISCARD);
  glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
  GLint pointsGenerated = -1;
  glGetQueryObjectiv(_vertQid, GL_QUERY_RESULT, &pointsGenerated);
  _vertCount += pointsGenerated; 

  // Print the number of verts collected.
  cout << "Vert Count: " << _vertCount << endl;

  // Draw the surface with associated poplights.
  GLuint el2Idx = glGetUniformLocation(_surfaceProg, "ElapsedSec");
  GLuint accIdx = glGetUniformLocation(_surfaceProg, "WorldAccel");
  GLuint mxaIdx = glGetUniformLocation(_surfaceProg, "MaxAgeSec");
  glUseProgram(_surfaceProg);
  glUniform1f(el2Idx, secondsElapsed);
  glUniform3f(accIdx, 0.0, -9.8, 0.0);
  glUniform1f(mxaIdx, 4.0f);

  glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, _vertQid);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _transformFeedback);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _worldPosBo[_destXfbBuff]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _velocityBo[_destXfbBuff]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, _ageBo[_destXfbBuff]);
    glBeginTransformFeedback(GL_POINTS);
  
    glBindVertexArray(_surfaceVao[_srcXfbBuff]);
    glDrawArrays(GL_POINTS, 0, _vertCount);
    glBindVertexArray(0);

    glEndTransformFeedback();
  glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
  GLint pointsProcessed = -1;
  glGetQueryObjectiv(_vertQid, GL_QUERY_RESULT, &pointsProcessed);
  _vertCount = pointsProcessed; 

  cout << "Vert Count: " << _vertCount << endl;

  // Swap source and destination buffers.
  _srcXfbBuff = 1 - _srcXfbBuff;
  _destXfbBuff = 1 - _destXfbBuff;

  checkGLError();

  // Accumulate time.
  _time += secondsElapsed;
}

void Cube::checkGLError() {
  GLenum error = GL_NO_ERROR;
  do {
    error = glGetError();
    switch (error) {
    case GL_NO_ERROR:
      break;
    case GL_INVALID_ENUM:
      cerr << "GLERROR: INVALID_ENUM" << endl;
      break;
    case GL_INVALID_VALUE:
      cerr << "GLERROR: INVALID_VALUE" << endl;
      break;
    case GL_INVALID_OPERATION:
      cerr << "GLERROR: INVALID_OPERATION" << endl;
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      cerr << "GLERROR: INVALID_FRAMEBUFFER_OPERATION" << endl;
      break;
    case GL_OUT_OF_MEMORY:
      cerr << "GLERROR: OUT_OF_MEMORY" << endl;
      break;
    default:
      cerr << "GLERROR: UNKNOWN ERROR" << endl;
    }
  } while (error != GL_NO_ERROR);

  return;
}
