#include "VertexArray.h"

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &VAO);
    this->bind();
    
}

VertexArray::~VertexArray()
{
    //
}

void VertexArray::bind()
{
    glBindVertexArray(VAO);
}
