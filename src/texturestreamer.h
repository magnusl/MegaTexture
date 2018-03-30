#ifndef TEXTURESTREAMER_H
#define TEXTURESTREAMER_H

#include "request.h"
#include <queue>
#include <Windows.h>
#include "megatexture.h"

namespace mt {

struct StreamStatus
{
    enum {
        kSuccess,
        kInvalidIdentifier,
        kError
    } Status;

    uint32_t    Key;
    uint8_t *   Data;
};

///
/// \class  TextureStreamer
///
class TextureStreamer
{
public:
    TextureStreamer(MegaTexture &,
        const char * TextureFile, size_t NumThreads);
    ~TextureStreamer();

    /// Submit a request for a page
    void Submit(const mt::PageRequest_t &);

    /// Get completed jobs
    bool GetCompleted(std::vector<StreamStatus> &);

    /// Win32 thread function
    static DWORD CALLBACK ThreadFunc(LPVOID);

protected:
    bool Fetch(PageRequest_t &);
    bool Complete(StreamStatus &);

private:
    std::vector<HANDLE>             _threads;
    std::queue<PageRequest_t>       _requests;      //< page requests
    std::vector<StreamStatus>       _completed;     //< completed requests
    MegaTexture &                   _megaTexture;
    std::string                     _filename;
    CRITICAL_SECTION                _lock;
};

} // namespace mt

#endif
