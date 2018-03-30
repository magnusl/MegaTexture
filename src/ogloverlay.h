#ifndef OGLOVERLAY_H
#define OGLOVERLAY_H

#include "oglbufferedtexture.h"
#include "oglshader.h"
#include "oglvertexbuffer.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

namespace ogl {

///
/// \class  Overlay
///
class Overlay
{
public:
    Overlay(size_t w, size_t h);
    bool ShowTexture(float x, float y, float w, float h, ogl::BufferedTexture &);
    bool ShowTexture(float x, float y, float w, float h, GLuint);

protected:
    glm::mat4                           _projectionMatrix;
    std::shared_ptr<ogl::ShaderProgram> _shader;
    std::shared_ptr<ogl::VertexBuffer>  _buffer;
    size_t                              _w, _h;
};

} // namespace ogl

#endif
