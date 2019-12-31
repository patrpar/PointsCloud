#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
public:
    GLuint program;


    Shader() = delete;
    Shader(std::string& vertexShaderPath, std::string& fragmentShaderPath);
    Shader(Shader& shader) = delete;
    Shader(Shader&& shader) = delete;
    Shader& operator() (Shader& shader) = delete;

    void use();

    void setUniformMat4f(const std::string& name, glm::mat4& mat4);

private:
    std::string readFileAsString(std::string& path);
    GLuint createShader(std::string& source, GLenum type);
    GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);
};

