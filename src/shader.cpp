#include "shader.hpp"

#include <iostream>

Shader &Shader::Use() {
  glUseProgram(this->ID);
  return *this;
}


void Shader::Compile(const char* vertexSource, const char* geometrySource, const char* fragmentSource) {
  unsigned int sVertex, sFragment, sGeometry;

  sVertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(sVertex, 1, &vertexSource, NULL);
  glCompileShader(sVertex);
  checkCompileErrors(sVertex, "VERTEX");

  sFragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(sFragment, 1, &fragmentSource, NULL);
  glCompileShader(sFragment);
  checkCompileErrors(sFragment, "FRAGMENT");

  if (geometrySource != nullptr) {
    sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(sGeometry, 1, &geometrySource, NULL);
    glCompileShader(sGeometry);
    checkCompileErrors(sGeometry, "GEOMETRY");
  }

  this->ID = glCreateProgram();
  glAttachShader(this->ID, sVertex);
  glAttachShader(this->ID, sFragment);
  if (geometrySource != nullptr) {
    glAttachShader(this->ID, sGeometry);
  }
  glLinkProgram(this->ID);
  checkCompileErrors(this->ID, "PROGRAM");

  glDeleteShader(sVertex);
  glDeleteShader(sFragment);
  if (geometrySource != nullptr)
    glDeleteShader(sGeometry);
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource) {
  Shader::Compile(vertexSource, nullptr, fragmentSource);
}

void Shader::SetFloat(const char *name, float value, bool useShader) {
  if(useShader)
    this->Use();
  glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetInteger(const char *name, int value, bool useShader) {
  if(useShader)
    this->Use();
  glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetVec2(const char *name, const glm::vec2 &value, bool useShader) {
  if(useShader)
    this->Use();
  glUniform2fv(glGetUniformLocation(this->ID, name), 1, &value[0]);
}

void Shader::SetVec3(const char *name, const glm::vec3 &value, bool useShader) {
  if(useShader)
    this->Use();
  glUniform3fv(glGetUniformLocation(this->ID, name), 1, &value[0]);
}

void Shader::SetVec4(const char *name, const glm::vec4 &value, bool useShader) {
  if(useShader)
    this->Use();
  glUniform4fv(glGetUniformLocation(this->ID, name), 1, &value[0]);
}

void Shader::SetMat4(const char *name, const glm::mat4 &value, bool useShader) {
  if(useShader)
    this->Use();
  glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(value));
}


void Shader::checkCompileErrors(unsigned int object, std::string type) {
  int success;
  char infoLog[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(object, 1024, NULL, infoLog);
      std::cerr << "| Error: Could not compile shader of type: " << type << "\n"
        << infoLog << "\n-----------------------------------------------\n";
    }
  }
  else {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(object, 1024, NULL, infoLog);
      std::cerr << "| Error: Could not link the shader program " << "\n"
        << infoLog << "\n-----------------------------------------------\n";
    }
  }
}
