#include "ogloverlay.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <glm/glm.hpp>

namespace ogl {

static float fullscreenQuad[] = {
    0.0f, 0.0f, 0.0f,0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,0.0f, 1.0f
};

Overlay::Overlay(size_t w, size_t h) : _w(w), _h(h)
{
    _projectionMatrix = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    _buffer = std::make_shared<ogl::VertexBuffer>(fullscreenQuad, sizeof(fullscreenQuad), ogl::VertexBuffer::V3_T2);
    _shader = ogl::ShaderProgram::LoadProgramFromFiles(
        "g:\\shaders\\identity.vs",
        "g:\\shaders\\texture.fs"
    );
}


bool Overlay::ShowTexture(float x, float y, float w, float h,
    ogl::BufferedTexture & Texture)
{
    return ShowTexture(x, y, w, h, Texture.texture());
}

bool Overlay::ShowTexture(float x, float y, float w, float h, GLuint Texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glm::mat4 tran  = glm::translate(glm::mat4(1.0), glm::vec3(x, y, 1));
    glm::mat4 mv    = glm::scale(glm::mat4(1.0), glm::vec3(w, h, 1.0));
    glm::mat4 mvp   = _projectionMatrix * mv * tran;

    glViewport(0, 0, _w, _h);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(_shader->id());
    glBindBuffer(GL_ARRAY_BUFFER, _buffer->id());
    int uMVP = glGetUniformLocation(_shader->id(), "MVP");
    glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    assert(glGetError() == 0);
    // texture coordinate
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(3 *sizeof(float)));
    assert(glGetError() == 0);

    int textureCache    = glGetUniformLocation(_shader->id(), "texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(textureCache, 0);

    glDrawArrays(GL_TRIANGLES, 0, _buffer->size() / (sizeof(float) * 5));
    assert(glGetError() == 0);
    // disable attribute streams
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    return true;
}

} // namespace ogl
