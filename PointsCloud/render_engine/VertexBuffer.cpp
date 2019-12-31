#include "VertexBuffer.h"

#include <glad/glad.h>

VertexBuffer::VertexBuffer(std::vector<float>& vertices) {


    size = vertices.size() / 6;
    glCreateBuffers(1, &VBO);
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices.front(), GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
  // TODO
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint VertexBuffer::getSize()
{
    return size;
}
