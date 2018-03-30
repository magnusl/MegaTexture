#ifndef OGL_SHADER_H
#define OGL_SHADER_H

#include <memory>
#include <string>
#include <gl\glew.h>
#include <glm\glm.hpp>

namespace ogl {

class ShaderProgram
{
public:
    ~ShaderProgram();

    void Enable(void);
    void Disable(void);

    int  GetAttributeLocation(const char *);
    bool SetModelViewProjection(const glm::mat4 &);
    bool SetModelView(const glm::mat4 &);
    bool SetVector3f(const char *, const glm::vec3 &);
    bool SetMatrix4(const char *, const glm::mat4 &);

    static std::shared_ptr<ShaderProgram>   
        LoadProgramFromFiles(const char *, const char *);

    static std::shared_ptr<ShaderProgram>   
        LoadProgramFromSources(const char *, const char *);

    GLint   id() const  { return m_hProgram; }

protected:
    ShaderProgram(GLint);
    static bool LoadSourceFile(const char *, std::string &);

private:
    GLint   m_hProgram;
};

} // namespace ogl

#endif