#include "Shader.h"
#include <vector>
#include <iostream>

Shader::Shader(std::string& vertexShaderPath, std::string& fragmentShaderPath)
{
    // TODO add some try catch there

    auto vertexShaderSource = readFileAsString(vertexShaderPath);
    auto vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);

    auto fragmentShaderSource = readFileAsString(fragmentShaderPath);
    auto fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);


    program = createProgram(vertexShader, fragmentShader);
}

void Shader::use()
{
    glUseProgram(program);
}

void Shader::setUniformMat4f(const std::string& name, glm::mat4& mat4)
{
    // TODO cache uniform location maybe using map
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat4[0][0]);
}

std::string Shader::readFileAsString(std::string& path)
{
    std::stringstream fileBuffer;
    std::ifstream file(path);
    fileBuffer << file.rdbuf();

    file.close();
    return fileBuffer.str();
}

GLuint Shader::createShader(std::string& source, GLenum type)
{

    unsigned int shaderId = glCreateShader(type);
    const GLchar* sourceCasted = static_cast<const GLchar*>(source.c_str());
    glShaderSource(shaderId, 1, &sourceCasted, 0);

    glCompileShader(shaderId);

    GLint isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shaderId);

        std::cout << infoLog.data();

        throw std::runtime_error("Couldn't copile shader!");
        return -1;
    }

    return shaderId;
}

GLuint Shader::createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (!isLinked)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);


        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error("Couldn't create program!");

        return -1;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
