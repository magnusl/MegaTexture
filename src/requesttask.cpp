#include "requesttask.h"
#include <iostream>

using namespace std;

namespace mt {

RequestTask::RequestTask(TextureStreamer & Streamer,
    const std::vector<std::pair<uint32_t, size_t> > & Feedback,
    MemoryPool & Pool) :
    _streamer(Streamer),
    _feedback(Feedback),
    _pool(Pool)
{
    // empty
}

void RequestTask::Compute()
{
    if (!_feedback.empty()) {
        for(size_t i = 0, count = _feedback.size(); i < count; ++i) {
            
            //std::cout << "feedback = " << _feedback.size() << std::endl;

            uint32_t key = _feedback[i].first;
            if (_history.find(key) != _history.end()) {
                continue;
            }

            PageRequest_t request;
            request.Storage     = _pool.Alllocate();
            request.StorageSize = _pool.blocksize();
            request.Key         = _feedback[i].first;
            if (!request.Storage) {     //< ran out of storage space, done for now
                return;
            }
            //std::cout << "Request: " << request.Key << std::endl;
            _streamer.Submit(request);  //< submit the job to the streamer
            _history.insert(key);
            //std::cout << "Request done." << std::endl;
        }
    }
}

} // namespace mt
