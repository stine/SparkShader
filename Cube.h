#ifndef __CUBE_H__
#define __CUBE_H__

class Cube {
 public:
  Cube();
  void render(float secondsElapsed);
 private:

  // OpenGL handles for uploaded model data
  unsigned int _vao;
  unsigned int _vertsVbo;
  unsigned int _colorsVbo;

  // OpenGL shader handle
  int _programHandle;

  // model data
  int    _numIndices;
  float *_verts3;
  float *_colors4;
};

#endif //__CUBE_H__
