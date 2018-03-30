#include "texturestreamer.h"
#include <stdexcept>
#include <turbojpeg.h>

namespace mt {

TextureStreamer::TextureStreamer(MegaTexture & Texture,
    const char * TextureFile, size_t NumThreads) :
    _megaTexture(Texture)
{
    // initialize data
    ::InitializeCriticalSection(&_lock);
    _filename = TextureFile;

    // create threads
    for(size_t i = 0; i < NumThreads; ++i) {
        _threads.push_back(CreateThread(0, 0, TextureStreamer::ThreadFunc, this, 0, nullptr));
    }
}

TextureStreamer::~TextureStreamer()
{
    ::DeleteCriticalSection(&_lock);
}

void TextureStreamer::Submit(const mt::PageRequest_t & Request)
{
    ::EnterCriticalSection(&_lock);
    _requests.push(Request);
    ::LeaveCriticalSection(&_lock);
}

bool TextureStreamer::Fetch(PageRequest_t & request)
{
    bool ret = false;
    ::EnterCriticalSection(&_lock);
    if (!_requests.empty()) {
        request = _requests.front(); _requests.pop();
        ret = true;
    }
    ::LeaveCriticalSection(&_lock);
    return ret;
}

bool TextureStreamer::Complete(StreamStatus & c)
{
    ::EnterCriticalSection(&_lock);
    _completed.push_back(c);
    ::LeaveCriticalSection(&_lock);
    return true;
}

bool TextureStreamer::GetCompleted(std::vector<StreamStatus> & Completed)
{
    ::EnterCriticalSection(&_lock);
    Completed = _completed;
    _completed.clear();
    ::LeaveCriticalSection(&_lock);

    return Completed.empty() == false;
}

DWORD CALLBACK TextureStreamer::ThreadFunc(LPVOID arg)
{
    TextureStreamer * instance = (TextureStreamer *) arg;
    if (instance) {
        // open the file to stream from
        FILE * file = fopen(instance->_filename.c_str(), "rb");
        if (!file) {
            return 0;
        }
        
        tjhandle handle = tjInitDecompress();
        while(1)
        {
            PageRequest_t request;
            if (instance->Fetch(request)) {
                const PageEntry_t * item = instance->_megaTexture.Find(request.Key);
                if (item) {
                    int ret = _fseeki64(file, item->Offset, SEEK_SET);
                    if (ret == 0) {
                        if (item->CompressedSize) {
                            std::vector<uint8_t> data(item->CompressedSize);
                            if (fread(&data[0], item->CompressedSize, 1, file) == 1) {
                                int sub, w, h;
                                if (!tjDecompressHeader2(handle, &data[0],data.size(),&w, &h, &sub))
                                {
                                    if (w == 128 && h == 128) {
                                        if (!tjDecompress(handle, &data[0], data.size(),(unsigned char *) request.Storage, w, w*3, h, 3, 0)) {
                                            StreamStatus status;
                                            status.Data = (uint8_t *)request.Storage;
                                            status.Key  = request.Key;
                                            status.Status = StreamStatus::kSuccess;
                                            instance->Complete(status);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // release resources
        fclose(file);
    }

    return 0;
}

} // namespace mt
