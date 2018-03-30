#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "task.h"
#include "oglshader.h"
#include "oglvertexbuffer.h"
#include "oglbufferedtexture.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <memory>

namespace mt {

///
/// \brief  Renders the scene using the MegaTexture
///
class RenderTask : public tasks::TaskNode
{
public:
    RenderTask(size_t W, size_t H, const ogl::VertexBuffer & Scene,
        const glm::mat4 & ModelViewProjection,
        const ogl::BufferedTexture & TextureCache,
        const ogl::BufferedTexture & PageTable);

    /// does the actual work
    virtual void Compute();

private:
    std::shared_ptr<ogl::ShaderProgram> _shader;
    const ogl::VertexBuffer &           _scene;
    const ogl::BufferedTexture &        _texture;
    const ogl::BufferedTexture &        _pagetable;

    const glm::mat4 &   _mvp;   //< Model View Projection matrix
    size_t              _w;
    size_t              _h;
};

} // namespace mt

#endif
