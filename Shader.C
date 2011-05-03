// Note: this file is my shameful hack. :(

#include "Shader.h"
#include <GL/gl.h>
#include <iostream>
#include <stdio.h>

#define GL_TESS_CONTROL_SHADER    0x8E88
#define GL_TESS_EVALUATION_SHADER 0x8E87

char * shader::fileRead (const char *fn) {
  FILE *fp;
  char *content = NULL;
  
  int count = 0;
  
  if (fn != NULL) {
    fp = fopen(fn,"rt");

    if (fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);
      
      if (count > 0) {
	content = new char[sizeof(char) * (count+1)];
	count = fread(content,sizeof(char),count,fp);
	content[count] = '\0';
      }

      fclose(fp);
    }
  }

  return content;
}

bool shader::compileShader(const char *shaderText, GLenum shaderType, GLuint &handle)
{
  GLint compileSuccess;
  GLchar compilerSpew[256];

  char shaderTypeText[16];
  switch (shaderType) {
  case GL_VERTEX_SHADER:
    snprintf(shaderTypeText, 16, "Vertex");
    break;
  case GL_TESS_CONTROL_SHADER:
    snprintf(shaderTypeText, 16, "TessControl");
    break;
  case GL_TESS_EVALUATION_SHADER:
    snprintf(shaderTypeText, 16, "TessEvaluation");
    break;
  case GL_GEOMETRY_SHADER_ARB:
    snprintf(shaderTypeText, 16, "Geometry");
    break;
  case GL_FRAGMENT_SHADER:
    snprintf(shaderTypeText, 16, "Fragment");
    break;
  default:
    fprintf(stderr, "ERROR: Invalid shader type.\n");
    return false;
    break;
  }
  
  handle = glCreateShader(shaderType);

  glShaderSource(handle, 1, &shaderText, 0);
  glCompileShader(handle);
  glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);
  glGetShaderInfoLog(handle, sizeof(compilerSpew), 0, compilerSpew);
  if (!compileSuccess) {
    fprintf(stderr, "%s Shader Compiling:\n%s\n", shaderTypeText, compilerSpew);
    return false;
  }

  return true;
}


// NOTE: Currently only supports vertex and fragment shaders.
GLint shader::loadEffect(const char *vS,
			 const char *tcS,
			 const char *teS,
			 const char *gS,
			 const char *fS)
{
  GLuint programHandle = 0;
  GLint linkSuccess;
  GLchar compilerSpew[256];
  
  // Load source for each shader
  const char* vsSource = fileRead(vS);
  // const char* tcsSource = fileRead(tcS);
  // const char* tesSource = fileRead(teS);
  // const char* gsSource = fileRead(gS);
  const char* fsSource = fileRead(fS);
  
  // Compile shader obejcts for each shader
  GLuint vsHandle;
  GLuint fsHandle;
  compileShader(vsSource, GL_VERTEX_SHADER, vsHandle);
  compileShader(fsSource, GL_FRAGMENT_SHADER, fsHandle);

  // Link the shaders
  programHandle = glCreateProgram();
  glAttachShader(programHandle, vsHandle);
  // glAttachShader(programHandle, tcsHandle);
  // glAttachShader(programHandle, tesHandle);
  // glAttachShader(programHandle, gsHandle);
  glAttachShader(programHandle, fsHandle);
  glLinkProgram(programHandle);
  glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
  glGetProgramInfoLog(programHandle, sizeof(compilerSpew), 0, compilerSpew);
  if (!linkSuccess) {
    fprintf(stderr, "ERROR: Shader Linking:\n%s", compilerSpew);
  }
  
  return programHandle;
}
