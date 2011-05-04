// Shader.C
// Author: Matt Stine


#include <GL/glew.h>
#include <string>
#include <iterator>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "Shader.h"

using namespace std;

string shader::readShaderFile (string fileName)
{
  // Check for empty filename string.
  if (!fileName.size()) {
    throw(invalid_argument("Empty shader filename string."));
  }
  
  // Attempt to open file.
  ifstream is(fileName.c_str());
  is.unsetf(ios::skipws);
  istream_iterator<char> ii(is);
  istream_iterator<char> eos;
  if (!is) {
    throw(runtime_error("Could not open file " + fileName));
  }
  
  // Load file.
  string content(ii, eos);
  is.close();
  if (!content.size()) {
    throw(runtime_error("Empty file " + fileName));
  }
  
  return content;
}


GLuint shader::compileShader(GLenum shaderType,
			     string shaderText)
{
  // Determine the type of shader being created.
  string shaderTypeText;
  switch (shaderType) {
  case GL_VERTEX_SHADER:
    shaderTypeText = "Vertex";
    break;
  case GL_TESS_CONTROL_SHADER:
    shaderTypeText = "TessControl";
    break;
  case GL_TESS_EVALUATION_SHADER:
    shaderTypeText = "TessEvaluation";
    break;
  case GL_GEOMETRY_SHADER:
    shaderTypeText = "Geometry";
    break;
  case GL_FRAGMENT_SHADER:
    shaderTypeText = "Fragment";
    break;
  default:
    throw(invalid_argument("Invalid shader type"));
  }

  // Ensure we're provided with valid shader text.
  if (!shaderText.size()) {
    throw(invalid_argument(shaderTypeText + " shader - empty shader text"));
  }

  // Create the requested type of shader.
  GLuint shaderHandle = glCreateShader(shaderType);
  if (!glIsShader(shaderHandle)) {
    throw(runtime_error(shaderTypeText + " shader - failed to create handle"));
  }

  // Compile the shader.
  GLint compileSuccess;
  GLchar compilerSpew[256];
  const char *text = shaderText.c_str();
  glShaderSource(shaderHandle, 1, &text, 0);
  glCompileShader(shaderHandle);
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
  glGetShaderInfoLog(shaderHandle, sizeof(compilerSpew), 0, compilerSpew);
  if (!compileSuccess) {
    glDeleteShader(shaderHandle);
    throw(runtime_error(shaderTypeText + " shader - compilation error:\n" +
			compilerSpew));
  }

  return shaderHandle;
}


// NOTE: Currently only supports vertex and fragment shaders.
bool shader::loadShader(const char *vS,
			const char *tcS,
			const char *teS,
			const char *gS,
			const char *fS,
			GLuint &programHandle,
			bool particleGen)
{
  GLint  linkSuccess = 1;
  GLchar linkerSpew[256];
  
  // Load source for each shader
  string vsSource;
  string fsSource;
  string tcsSource;
  string tesSource;
  string gsSource;
  GLuint vsHandle  = 0;
  GLuint fsHandle  = 0;
  GLuint tesHandle = 0;
  GLuint tcsHandle = 0;
  GLuint gsHandle  = 0;

  try {
    // Create a program handle.
    programHandle = glCreateProgram();
    if (!glIsProgram(programHandle)) {
      throw(runtime_error("Failed to create shader program handle"));
    }
    
    // Read, compile, and attach each shader object to the program.
    vsSource = readShaderFile(vS);
    vsHandle = compileShader(GL_VERTEX_SHADER, vsSource);
    glAttachShader(programHandle, vsHandle);
    
    fsSource = readShaderFile(fS);
    fsHandle = compileShader(GL_FRAGMENT_SHADER, fsSource);
    glAttachShader(programHandle, fsHandle);
    
    if (tcS) {
      tcsSource = readShaderFile(tcS);
      tcsHandle = compileShader(GL_TESS_CONTROL_SHADER, tcsSource);
      glAttachShader(programHandle, tcsHandle);
    }
    
    if (teS) {
      tesSource = readShaderFile(teS);
      tesHandle = compileShader(GL_TESS_EVALUATION_SHADER, tesSource);
      glAttachShader(programHandle, tesHandle);
    }
    
    if (gS) {
      gsSource = readShaderFile(gS);
      gsHandle = compileShader(GL_GEOMETRY_SHADER, gsSource);
      glAttachShader(programHandle, gsHandle);
    }

    // TODO: hardcoded hack for poplights
    // Identify varyings to be captured by transform feedback.
    if (particleGen) {
      glBindAttribLocation(programHandle, 0, "position");
      glBindFragDataLocation(programHandle, 0, "fragcolor");

      const GLchar *varyings[] = {"ptPosition", "ptMaxRadius", "ptAgeSeconds"};
      glTransformFeedbackVaryings(programHandle, 3, varyings, GL_SEPARATE_ATTRIBS);
    }
    else {
      glBindAttribLocation(programHandle, 0, "position");
      glBindAttribLocation(programHandle, 1, "maxRadius");
      glBindAttribLocation(programHandle, 2, "ageSeconds");
      glBindFragDataLocation(programHandle, 0, "fragcolor");

      const GLchar *varyings[] = {"ptPosition", "ptMaxRadius", "ptAgeSeconds"};
      glTransformFeedbackVaryings(programHandle, 3, varyings, GL_SEPARATE_ATTRIBS);
    }

    // Link the shader program.
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(programHandle, sizeof(linkerSpew), 0, linkerSpew);
    if (!linkSuccess) {
      throw(runtime_error("Shader program linking error:\n" +
			  string(linkerSpew)));
    }
  }
  catch (exception &e) {
    // Handle errors.
    glDeleteShader(vsHandle);
    glDeleteShader(fsHandle);
    glDeleteShader(tesHandle);
    glDeleteShader(tcsHandle);
    glDeleteShader(gsHandle);
    glDeleteProgram(programHandle);
    programHandle = 0;

    cerr << "ERROR creating shader program: " << e.what() << endl;
    return false;
  }
  
  return true;
}
