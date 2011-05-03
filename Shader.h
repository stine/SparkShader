#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/gl.h>

namespace shader {
  int loadEffect(const char *vS,
		 const char *tcS,
		 const char *teS,
		 const char *gS,
		 const char *fS);
  char  *fileRead(const char *fileName);
  bool compileShader(const char *shaderText, GLenum shaderType, GLuint &handle);
}

#endif //__SHADER_H__
