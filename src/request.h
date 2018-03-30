#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

namespace mt {

inline uint32_t GetKey(uint8_t MipLevel, uint8_t X, uint8_t Y)
{
    return (static_cast<uint32_t>(MipLevel) << 24) |
        (static_cast<uint32_t>(X) << 8) | Y;
}

///
/// \brief  Request for a specific page
///
struct PageRequest_t
{
    uint32_t    Key;
    void *      Storage;        //< Storage to store the decompressed page
    size_t      StorageSize;    //< Size of the Storage buffer
};

inline uint32_t GetKey(const PageRequest_t & Request)
{
    return Request.Key;
}

} // namespace mt

#endif
