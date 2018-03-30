#include "oglbufferedtexture.h"
#include <stdexcept>
#include <gl\glew.h>
#include <assert.h>

namespace ogl {

bool PowerOfTwo(size_t N)
{
    return (N & (N-1)) == 0;
}

BufferedTexture::BufferedTexture(size_t W, size_t H, size_t Levels) :
    _w(W),
    _h(H)
{
    if (!PowerOfTwo(W) || !PowerOfTwo(H)) {
        throw std::runtime_error("Not a multiple of two.");
    }

    // initial data is 0, which is black
    std::vector<uint8_t> data(W * H * 3);

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (Levels > 1) {
        // for the page table we need to use GL_NEAREST_MIPMAP_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    } else {
        // for the texture cache we need to use GL_NEAREST for now.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }

    for(size_t i = 0; i < Levels; ++i) {
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
        W /= 2;
        H /= 2;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

BufferedTexture::~BufferedTexture()
{
    glDeleteTextures(1, &_id);
}

void BufferedTexture::Update(size_t X, size_t Y, size_t W, size_t H, uint8_t * Data, size_t Level)
{
    BufferedTexture::Change_t change = {X, Y, W, H, Level, Data};
    _changeset.push_back(change);
}

void BufferedTexture::Commit(mt::MemoryPool * Pool)
{
    if (_changeset.empty()) {
        return;
    }
    assert(glGetError() == 0);
    glBindTexture(GL_TEXTURE_2D, _id);
    for(size_t i = 0, count = _changeset.size(); i < count; ++i) {
        if (_changeset[i].data) {
            const Change_t & c = _changeset[i];
            if (c.level == 0) {
                assert(glGetError() == 0);
                // update the corresponding mipmap level
                glTexSubImage2D(GL_TEXTURE_2D, c.level, c.x, c.y, c.w, c.h, GL_RGB, GL_UNSIGNED_BYTE, c.data);
                assert(glGetError() == 0);
                // deallocate the memory
            }
            if (Pool) {
                Pool->Deallocate(_changeset[i].data);
            }
        }
    }
    assert(glGetError() == 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == 0);
}

} // namespace ogl
