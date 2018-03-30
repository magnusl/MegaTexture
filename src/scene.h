#ifndef SCENE_H
#define SCENE_H

#include "oglvertexbuffer.h"
#include <memory>

namespace mt {

///
/// \class  Scene
///
class Scene
{
public:
    Scene(const char *);
    ~Scene();
    
    const ogl::VertexBuffer & buffer() const { return *_buffer; }

private:
    std::unique_ptr<ogl::VertexBuffer> _buffer;
};

} // namespace mt

#endif
