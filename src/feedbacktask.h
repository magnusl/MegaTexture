#ifndef FEEDBACKTASK_H
#define FEEDBACKTASK_H

#include "task.h"
#include "oglvertexbuffer.h"
#include "oglshader.h"
#include <vector>
#include <stdint.h>
#include <map>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <GL\glew.h>
#include <memory>

namespace mt {

///
/// \class  FeedbackTask
/// \brief  Renders the scene to a framebuffer, and then
///         reads the result back.
///
class FeedbackTask : public tasks::TaskNode
{
public:
    FeedbackTask(const ogl::VertexBuffer & Scene,
        size_t W,
        size_t H,
        const glm::mat4 & ModelViewProjection);

    ~FeedbackTask();

    /// does the actual work
    virtual void Compute();

    /// return the texture that we rendered to
    GLuint texture() const { return _texture; }

    /// return the framebuffer data that we read
    const std::vector<uint8_t> & framebuffer() const { return _framebuffer; }

protected:
    std::shared_ptr<ogl::ShaderProgram> _shader;
    const ogl::VertexBuffer &           _scene;
    const glm::mat4 &                   _mvp;
    std::vector<uint8_t>                _framebuffer;
    size_t                              _w, _h;
    GLuint                              _fbo;           //< framebuffer object
    GLuint                              _texture;       //< texture that we render to
    GLuint                              _rbo;
};

} // namespace mt

#endif
