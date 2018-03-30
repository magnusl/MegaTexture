#include "feedbacktask.h"
#include <map>
#include <stdint.h>
#include <assert.h>

static const char * VertexShader    = "g:\\shaders\\identity.vs";
static const char * FragmentShader  = "g:\\shaders\\mipmap.fs";


namespace mt {

FeedbackTask::FeedbackTask(
    const ogl::VertexBuffer & Scene,
    size_t W,
    size_t H,
    const glm::mat4 & ModelViewProjection) :
    _scene(Scene),
    _mvp(ModelViewProjection),
    _w(W),
    _h(H)
{
    // load shader
    _shader = ogl::ShaderProgram::LoadProgramFromFiles(VertexShader, FragmentShader);
    if (!_shader) {
        throw std::runtime_error("Failed to load shader for feedback task.");
    }
    assert(glGetError() == 0);
    // create a RGB8 texture
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // create depth texture
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, W, H);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // create a framebuffer object
    glGenFramebuffers(1, &_fbo);                //< create a framebuffer object
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);    //< bind framebuffer
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &_fbo);     //< delete framebuffer object
        glDeleteRenderbuffers(1, &_rbo);
        glDeleteTextures(1, &_texture);     //< delete texture
        throw std::runtime_error("Failed to create framebuffer object.");
    }
    assert(glGetError() == 0);
    _framebuffer.resize(W * H * 3);
}

FeedbackTask::~FeedbackTask()
{
    glDeleteFramebuffers(1, &_fbo);
    glDeleteRenderbuffers(1, &_rbo);
    glDeleteTextures(1, &_texture);
}

void FeedbackTask::Compute()
{
    assert(glGetError() == 0);
    glEnable(GL_DEPTH_TEST);
    assert(glGetError() == 0);
    // render the scene to our framebuffer
    assert(glGetError() == 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);                //< bind framebuffer
    assert(glGetError() == 0);
    glViewport(0,0,_w, _h);                                 //< set viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     //< clear buffers
    assert(glGetError() == 0);
    glUseProgram(_shader->id());                            //< use specific shader
    assert(glGetError() == 0);
    glBindBuffer(GL_ARRAY_BUFFER, _scene.id());             //< bind vertex buffer
    assert(glGetError() == 0);
    // set the model view projection
    int uMVP = glGetUniformLocation(_shader->id(), "MVP");
    glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(_mvp));
    assert(glGetError() == 0);
    // vertex data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    assert(glGetError() == 0);
    // texture coordinate
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(3 *sizeof(float)));
    //assert(glGetError() == 0);
    // draw the actual scene
    glDrawArrays(GL_TRIANGLES, 0, _scene.size() / (sizeof(float) * 5));
    int err = glGetError();
    assert(err == 0);
    // disable attribute streams
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    assert(glGetError() == 0);
    // read back the framebuffer
    glReadPixels(0, 0, _w, _h, GL_RGB, GL_UNSIGNED_BYTE, &_framebuffer[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert(glGetError() == 0);
}

} // namespace mt
