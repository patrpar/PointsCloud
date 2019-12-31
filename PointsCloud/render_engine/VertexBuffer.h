#pragma once
#include <vector>
#include <glad/glad.h>
#include <vector>


class VertexBuffer
{
    GLuint size;
public:
    VertexBuffer(std::vector<float>& vertices);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    GLuint VBO;
    GLuint getSize();
};

