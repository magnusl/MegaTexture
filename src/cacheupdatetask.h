#ifndef CACHEUPDATETASK_H
#define CACHEUPDATETASK_H

#include "task.h"
#include "pagetable.h"
#include "lrucache.h"
#include "texturestreamer.h"
#include "oglbufferedtexture.h"
#include "memorypool.h"

namespace mt {

///
/// \class  CacheUpdateTask
///
class CacheUpdateTask : public tasks::TaskNode
{
public:
    CacheUpdateTask(PageTable & Pt,
        LRUCache<uint32_t> & Cache,
        TextureStreamer &,
        ogl::BufferedTexture &,
        ogl::BufferedTexture &,
        MemoryPool &);

    /// does the actual work
    virtual void Compute();

protected:
    PageTable &             _pageTable;
    LRUCache<uint32_t> &    _cache;
    TextureStreamer &       _streamer;
    ogl::BufferedTexture &  _texture;
    ogl::BufferedTexture &  _pagetable_texture;
    MemoryPool &            _pool;
};

} // namespace mt

#endif
