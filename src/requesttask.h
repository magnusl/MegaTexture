#ifndef REQUESTTASK_H
#define REQUESTTASK_H

#include "task.h"
#include "texturestreamer.h"
#include "request.h"
#include "memorypool.h"
#include <map>
#include <stdint.h>
#include <set>

namespace mt {

///
/// \class  RequestTask
/// \brief  Task that requests textures
///
class RequestTask : public tasks::TaskNode
{
public:
    RequestTask(TextureStreamer & Streamer,
        const std::vector<std::pair<uint32_t, size_t> > & Feedback,
        MemoryPool & Pool);

    /// does the actual work
    virtual void Compute();

private:
    TextureStreamer &                                   _streamer;
    const std::vector<std::pair<uint32_t, size_t> > &   _feedback;
    MemoryPool &                                        _pool;
    std::set<uint32_t>                                  _history;
};

} // namespace mt

#endif
