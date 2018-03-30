#include "rendertask.h"
#include <GL/glew.h>

namespace mt {

static const char * VertexShader    = "g:\\shaders\\identity.vs";
static const char * FragmentShader  = "g:\\shaders\\megatexture.fs";

RenderTask::RenderTask(size_t W, size_t H,
    const ogl::VertexBuffer & Scene,
    const glm::mat4 & ModelViewProjection,
    const ogl::BufferedTexture & Texture,
    const ogl::BufferedTexture & PageTable) :
    _w(W),
    _h(H),
    _scene(Scene),
    _mvp(ModelViewProjection),
    _texture(Texture),
    _pagetable(PageTable)
{
    // empty
    _shader = ogl::ShaderProgram::LoadProgramFromFiles(VertexShader, FragmentShader);
    if (!_shader) {
        throw std::runtime_error("Failed to load shader for feedback task.");
    }
}

void RenderTask::Compute()
{
    assert(glGetError() == 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert(glGetError() == 0);
    glViewport(0,0,_w, _h);                                 //< set viewport
    assert(glGetError() == 0);
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
    assert(glGetError() == 0);
    
    // TextureCache
    int textureCache    = glGetUniformLocation(_shader->id(), "page_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture.texture());
    glUniform1i(textureCache, 0);

    // PageTable
    int pageTable   = glGetUniformLocation(_shader->id(), "diffuse_tex");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _pagetable.texture());
    glUniform1i(pageTable, 1);

    assert(glGetError() == 0);
    // draw the actual scene
    glDrawArrays(GL_TRIANGLES, 0, _scene.size() / (sizeof(float) * 5));
    assert(glGetError() == 0);
    // disable attribute streams
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    assert(glGetError() == 0);
}

} // namespace mt
