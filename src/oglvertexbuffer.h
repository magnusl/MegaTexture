#ifndef OGL_VERTEXBUFFER_H
#define OGL_VERTEXBUFFER_H

namespace ogl {

///
/// \class  VertexBuffer
///
class VertexBuffer
{
public:
    enum Format {
        V3_T2,
        V3_N3,
        V3_T2_N3,
    };

    VertexBuffer(const void *, size_t, Format);
    ~VertexBuffer();

    Format      format() const  { return _format; }
    unsigned    id() const      { return _id; }
    size_t      size() const    { return _size; }

protected:
    Format      _format;
    unsigned    _id;
    size_t      _size;
};

} // namespace gl

#endif