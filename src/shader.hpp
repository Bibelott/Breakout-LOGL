#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
  unsigned int ID;

  Shader () { }

  Shader &Use();
  void Compile(const char *vertexSource, const char *fragmentSource);
  void Compile(const char *vertexSource, const char *geometrySource, const char *fragmentSource);
  void SetFloat(const char *name, float value, bool useShader = false);
  void SetInteger(const char *name, int value, bool useShader = false);
  void SetVec2(const char *name, const glm::vec2 &value, bool useShader = false);
  void SetVec3(const char *name, const glm::vec3 &value, bool useShader = false);
  void SetVec4(const char *name, const glm::vec4 &value, bool useShader = false);
  void SetMat4(const char *name, const glm::mat4 &value, bool useShader = false);

private:
  void checkCompileErrors(unsigned int object, std::string type);
};
