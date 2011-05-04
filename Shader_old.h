// Shader.C
// Author: Matt Stine

#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <string>
#include <stdexcept>

namespace shader {
  bool loadShader(const char *vS,
		  const char *tcS,
		  const char *teS,
		  const char *gS,
		  const char *fS,
		  GLuint &programHandle,
		  bool particleGen); // HACK
  GLuint compileShader(GLenum shaderType, std::string shaderText);
  std::string readShaderFile(std::string fileName);
}

#endif //__SHADER_H__
