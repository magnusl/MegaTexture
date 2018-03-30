#ifndef OGL_BUFFEREDTEXTURE_H
#define OGL_BUFFEREDTEXTURE_H

#include <vector>
#include <stdint.h>
#include "memorypool.h"
#include <gl\glew.h>

namespace ogl {

///
/// \class  BufferedTexture
/// \brief  Buffered RGB8 texture
///
class BufferedTexture
{
public:
    BufferedTexture(size_t W, size_t H, size_t Levels = 1);
    ~BufferedTexture();

    void Commit(mt::MemoryPool *);      //< commit changes to video ram
    void Update(size_t X, size_t Y, size_t W, size_t H, uint8_t * Data, size_t Level = 0);

    GLuint  texture() const { return _id; }

private:
    
    struct Change_t {
        size_t  x, y, w, h;
        size_t level;
        uint8_t * data;
    };

    std::vector<Change_t>   _changeset;     //< changed areas
    size_t                  _w;
    size_t                  _h;
    GLuint                  _id;
};

} // namespace ogl

#endif
