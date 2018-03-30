#include "oglshader.h"

#include <string>
#include <gl/glew.h>
#include <cstdio>
#include <vector>
#include <Windows.h>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp> 

using std::shared_ptr;
using std::string;

namespace ogl {

ShaderProgram::ShaderProgram(GLint hProgram) : m_hProgram(hProgram)
{
}

ShaderProgram::~ShaderProgram()
{
    if (m_hProgram != -1) {
        glDeleteProgram(m_hProgram);
    }
}

/**
 * \breif   Enables the shader program
 */
void ShaderProgram::Enable()
{
    //assert(glGetError() == 0);
    glUseProgram(m_hProgram);
    assert(glGetError() == 0);
}

/**
 * \brief   Disables the shader program
 */
void ShaderProgram::Disable()
{
    glUseProgram(0);
}

/**
 * \brief   Retrives the attribute location.
 */
int ShaderProgram::GetAttributeLocation(const char * name)
{
    assert(glIsProgram(m_hProgram));
    return glGetAttribLocation(m_hProgram, name);
}

/*************************************************************************/
/*                                  GLM                                  */
/*************************************************************************/
bool ShaderProgram::SetModelViewProjection(const glm::mat4 & mvp)
{
    int pos = glGetUniformLocation(m_hProgram, "mvpMatrix");
    if (pos == -1) {
        return false;
    }
    glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mvp));
    return true;
}

bool ShaderProgram::SetModelView(const glm::mat4 & mv)
{
    int pos = glGetUniformLocation(m_hProgram, "mvMatrix");
    if (pos == -1) {
        assert(glGetError() == 0);
        return false;
    }
    glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mv));
    return true;
}

bool ShaderProgram::SetVector3f(const char * name, const glm::vec3 & vec)
{
    int pos = glGetUniformLocation(m_hProgram, name);
    if (pos == -1) {
        return false;
    }
    glUniform3fv(pos, 3, glm::value_ptr(vec));
    return true;
}

bool ShaderProgram::SetMatrix4(const char * name, const glm::mat4 & mat)
{
    int pos = glGetUniformLocation(m_hProgram, name);
    if (pos == -1) {
        return false;
    }
    glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
    return true;
}

/**
 * \brief   Creates a GLSL shader program from a vertex shader and a fragment shader.
 */
shared_ptr<ShaderProgram>
ShaderProgram::LoadProgramFromFiles(const char * vertexShader, const char * fragmentShader)
{
    string vertexSource, fragmentSource;
    if ((LoadSourceFile(vertexShader, vertexSource) == false) ||
        (LoadSourceFile(fragmentShader, fragmentSource) == false))
    {
        return shared_ptr<ShaderProgram>(nullptr);
    }

    return LoadProgramFromSources(vertexSource.c_str(), fragmentSource.c_str());
}

shared_ptr<ShaderProgram>
ShaderProgram::LoadProgramFromSources(const char * pVertexSource, const char * pFragmentSource)
{
    GLint hVertexShader = -1, hFragmentShader = -1;
    GLint hProgram = -1;

    if (pVertexSource == nullptr || pFragmentSource == nullptr)
        return shared_ptr<ShaderProgram>(nullptr);

    // lambda function for cleanup
    auto cleanup = [&hVertexShader, &hFragmentShader, &hProgram]() {
        if (hVertexShader != -1)    glDeleteShader(hVertexShader);
        if (hFragmentShader != -1)  glDeleteShader(hFragmentShader);
        if (hProgram != -1) {
            glDeleteProgram(hProgram);
            hProgram = -1;
        }
    };

    GLint status;

    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(hVertexShader, 1, &pVertexSource, 0);
    assert(glGetError() == 0);
    glShaderSource(hFragmentShader, 1, &pFragmentSource, 0);
    assert(glGetError() == 0);

    if (hVertexShader == -1 || hFragmentShader == -1)
    {
        cleanup();
        return shared_ptr<ShaderProgram>(nullptr);
    }
    assert(glGetError() == 0);
    glCompileShader(hVertexShader);
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &status);
    assert(glGetError() == 0);
    if (status == GL_FALSE)     // failed to compile vertex shader
    {
        char infoLog[1024];
        glGetShaderInfoLog(hVertexShader, sizeof(infoLog), NULL, infoLog);
        MessageBoxA(0, infoLog, 0, 0);
        cleanup();
        return shared_ptr<ShaderProgram>(nullptr);
    }
    assert(glGetError() == 0);
    glCompileShader(hFragmentShader);
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &status);
    assert(glGetError() == 0);
    if (status == GL_FALSE) {   // failed to compile fragment shader
        char infoLog[1024];
        glGetShaderInfoLog(hFragmentShader, sizeof(infoLog), NULL, infoLog);
        MessageBoxA(0, infoLog, 0, 0);
        cleanup();
        return shared_ptr<ShaderProgram>(nullptr);
    }

    assert(glGetError() == 0);
    hProgram = glCreateProgram();
    glAttachShader(hProgram, hVertexShader);
    glAttachShader(hProgram, hFragmentShader);

    assert(glGetError() == 0);
    glBindFragDataLocation(hProgram, 0, "vFragColor");

    assert(glGetError() == 0);

    glLinkProgram(hProgram);
    assert(glGetError() == 0);
    glGetProgramiv(hProgram, GL_LINK_STATUS, &status);
    assert(glGetError() == 0);

    // we don't need the shaders anymore
    if (hVertexShader != -1) {
        glDeleteShader(hVertexShader);
        hVertexShader = -1;
    }
    if (hFragmentShader != -1) {
        glDeleteShader(hFragmentShader);
        hFragmentShader = -1;
    }
    if (status == GL_FALSE)     // linking failed.
    {
        char infoLog[1024];
        glGetShaderInfoLog(hProgram, sizeof(infoLog), NULL, infoLog);
        MessageBoxA(0, infoLog, 0, 0);
        cleanup();
        return shared_ptr<ShaderProgram>(nullptr);
    }

    return shared_ptr<ShaderProgram>(new ShaderProgram(hProgram));
}

/**
 * \breif   Loads the shader source from a file into a string.
 */
bool ShaderProgram::LoadSourceFile(const char * filename, string & source)
{
    FILE * file = fopen(filename, "rb");
    if (!file)
        return false;

    std::vector<char>   data;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    data.resize(size + 1);
    fread(&data[0], 1, size + 1, file);
    data[size] = 0;
    fclose(file);

    source = &data[0];
    return true;
}

} // namespace gl
