#include "oglvertexbuffer.h"
#include <GL\glew.h>

namespace ogl {

VertexBuffer::VertexBuffer(const void * Source, size_t Size, Format Format) :
    _format(Format),
    _size(Size)
{
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, Size, Source, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_id);
}

} // namespace gl
