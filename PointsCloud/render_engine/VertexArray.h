#pragma once
#include <glad/glad.h>

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void bind();
  
    GLuint VAO;
};

