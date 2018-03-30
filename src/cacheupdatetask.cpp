#include "cacheupdatetask.h"
#include <iostream>
using namespace std;

namespace mt {

CacheUpdateTask::CacheUpdateTask(PageTable & Pt, 
    LRUCache<uint32_t> & Cache,
    TextureStreamer & Streamer,
    ogl::BufferedTexture & Texture,
    ogl::BufferedTexture & PageTableTexture,
    MemoryPool & Pool) :
    _pageTable(Pt),
    _cache(Cache),
    _streamer(Streamer),
    _texture(Texture),
    _pagetable_texture(PageTableTexture),
    _pool(Pool)
{
    // empty
}

void CacheUpdateTask::Compute()
{
    // get any finished streaming request
    vector<StreamStatus> completed;
    if (_streamer.GetCompleted(completed)) {                            //< There is one or more completed pages
        for(size_t i = 0, count = completed.size(); i < count; ++i)     //< for each page 
        {
            const StreamStatus & response = completed[i];

            if (StreamStatus::kSuccess == response.Status) {
                uint32_t evicted;                                           //< evicted item

                size_t slot = _cache.Put(response.Key, evicted);
                if (evicted != static_cast<size_t>(-1)) {                   //< a entry was evicted
                    _pageTable.Unmap(evicted);                              //< unmap the evicted one from the page table
                }

                size_t x = (slot / 32);
                size_t y = (slot % 32);

                _pageTable.Map(response.Key, slot);                         //< update the page table

                std::cout << "Update, x: " << x * 128 << " y: " << y * 128 << std::endl;
                _texture.Update(x * 128, y * 128, 128, 128, response.Data, 0);          //< update texture
            } else {
                _pool.Deallocate(response.Data);
            }
        }
        _pageTable.Commit(_pagetable_texture);      //< commit the changes to the page table
        _texture.Commit(&_pool);                    //< commit the actual changes, and deallocate memory
    }
}

} // namespace mt
