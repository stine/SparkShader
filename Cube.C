#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <cmath>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_operation.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/inverse_transpose.hpp>

#include "config.h"
#include "Cube.h"
#include "Shader.h"

Cube::Cube()
  : _vao(0),
    _vertsVbo(0),
    _colorsVbo(0),
    _programHandle(0),
    _numIndices(0),
    _verts3(NULL),
    _colors4(NULL)
{

  // initialize OpenGL state
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);

  // Load Shaders and use the loaded program.
  const char *vS = "shaders/cube.vs";
  const char *fS = "shaders/cube.fs";
  _programHandle = shader::loadEffect(vS, NULL, NULL, NULL, fS);
  glUseProgram(_programHandle);

  // Setup projection and modelview matrix
  glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
  glm::mat4 ModelViewMatrix = glm::mat4(1.0f);
  glm::mat3 NormalMatrix = glm::mat3(1.0f);
  ProjectionMatrix *= glm::perspective(25.0f,(float) (RENDER_WIDTH / RENDER_HEIGHT), 1.0f, 65.0f);
  ModelViewMatrix *= glm::lookAt(glm::vec3(405.0, 40.0, -7.0),
				 glm::vec3(440.0,  5.0,  0.0),
				 glm::vec3(  0.0,  1.0,  0.0));
  ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(450.0, 0.0, 0.0));

  /*
  glUniformMatrix4fv(glGetUniformLocation(ProgramHandle, "modelViewMatrix"),  1, false, &ModelViewMatrix[0][0]);
  glUniformMatrix3fv(glGetUniformLocation(ProgramHandle, "normalMatrix"),     1, false, &NormalMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(ProgramHandle, "projectionMatrix"), 1, false, &ProjectionMatrix[0][0]);
  */

  // TODO  glBindVertexArray(0);

  // TODO  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // TODO  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  //  glDeleteBuffers(1, &m_vboID);
  //  glDeleteVertexArrays(1, &m_vaoID);







#define FRAGCOLOR_NUMBER 0
#define POSITION_INDEX 0
#define COLOR_INDEX 1

  // TODO hack here:
  glBindAttribLocation(_programHandle, POSITION_INDEX, "position");
  glBindAttribLocation(_programHandle, COLOR_INDEX,   "color");
  glBindFragDataLocation(_programHandle, FRAGCOLOR_NUMBER, "fragcolor");
  




  // bind a vertex array object
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  
  // bind/populate the vertex vbo
  glGenBuffers(1, &_vertsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vertsVbo);
  glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);//_numIndices * 3 * sizeof(GLfloat), _verts3, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // bind/populate the normals vbo
  glGenBuffers(1, &_colorsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, _colorsVbo);
  glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);//_numIndices * 4 * sizeof(GLfloat), _colors4, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  return;
}

void Cube::render(float secondsElapsed) {

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // rotate cube
  // TODO
  
  // draw cube
  glDrawArrays(GL_TRIANGLES, 0, _numIndices);
}
