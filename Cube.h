#ifndef __CUBE_H__
#define __CUBE_H__

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"

class Cube {
 public:
  Cube();
  virtual ~Cube();
  void render(float secondsElapsed);
 private:

  void checkGLError();

  // OpenGL handles for uploaded model data
  GLuint _transformFeedback;
  GLuint _srcXfbBuff;
  GLuint _destXfbBuff;
  GLuint _worldPosBo[2];
  GLuint _velocityBo[2];
  GLuint _ageBo[2];
  GLuint _particleGenVao;
  GLuint _surfaceVao[2];
  GLuint _indicesVbo;
  GLuint _vertsVbo;
  
  GLuint _vertQid;
  GLuint _vertCount;

  // Shaders
  Shader _genProg;
  Shader _surfaceProg;
  
  // Time
  float _time;

  // model data
  std::vector<unsigned> _indices;
  std::vector<float>    _verts3;
  std::vector<float>    _colors4;

  // view data
  glm::mat4 _modelViewMatrix;
  glm::mat4 _projectionMatrix;
};

#endif //__CUBE_H__
