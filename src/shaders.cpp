// Copyright (c) 2025 Ewan Crawford
#include "shaders.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace {
void createShader(std::string Filename, GLuint ShaderID) {
  // Read shader from file
  // CMake copies shaders to <build_dir>/shaders/
  std::string ShaderPath = std::string("shaders/") + Filename;
  std::ifstream ShaderStream(ShaderPath, std::ios::in);
  if (!ShaderStream.is_open()) {
    throw std::runtime_error(std::string("Could not open shader ") + Filename);
  }

  std::stringstream sstr;
  sstr << ShaderStream.rdbuf();
  ShaderStream.close();

  // Compile Shader
  std::string ShaderCode = sstr.str();
  char const *ShaderSourceCStr = ShaderCode.c_str();
  glShaderSource(ShaderID, 1, &ShaderSourceCStr, nullptr);
  glCompileShader(ShaderID);

  // Check Shader
  GLint Result = GL_FALSE;
  glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
  if (Result != GL_TRUE) {
    int InfoLogLength;
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
      std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
      glGetShaderInfoLog(ShaderID, InfoLogLength, nullptr,
                         &ShaderErrorMessage[0]);

      const char *ErrorLogCStr = ShaderErrorMessage.data();
      throw std::runtime_error(std::string("shader not compiled: ") +
                               std::string(ErrorLogCStr));
    } else {
      throw std::runtime_error("Shader not compiled");
    }
  }
}

GLuint loadShaders(std::string VertexShader, std::string FragShader) {
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  createShader(VertexShader, VertexShaderID);

  GLuint FragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  createShader(FragShader, FragShaderID);

  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragShaderID);
  glLinkProgram(ProgramID);

  GLint Result = GL_FALSE;
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {
    int InfoLogLength;
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
      std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
      glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr,
                          &ProgramErrorMessage[0]);
      const char *ErrorLogCStr = ProgramErrorMessage.data();
      throw std::runtime_error(std::string("Shader program could not link: ") +
                               std::string(ErrorLogCStr));
    } else {
      throw std::runtime_error("Shader program could not link");
    }
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragShaderID);

  return ProgramID;
}

} // namespace

GLuint loadSphereShaders() {
  return loadShaders("sphere_vertex.glsl", "sphere_frag.glsl");
}

GLuint loadSkyboxShaders() {
  return loadShaders("skybox_vertex.glsl", "skybox_frag.glsl");
}

GLuint loadTextShaders() {
  return loadShaders("font_vertex.glsl", "font_frag.glsl");
}
