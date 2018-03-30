#ifndef MEGATEXTURE_H
#define MEGATEXTURE_H

#include <stdint.h>
#include <vector>

namespace mt {

struct PageEntry_t {
    uint32_t    PageId;             //< id of page, mip-level + x,y
    uint32_t    CompressedSize;     //< size of the compressed page
    uint64_t    Offset;             //< offset to page from the start of the file
};

struct FileHeader_t {
    char        Id[4];              //< _MT_
    uint16_t    VirtualSize;        //< virtual size, power of two, in kibibytes
    uint16_t    PageSize;           //< Page size, power of two
};

///
/// \class  MegaTexture
///
class MegaTexture
{
public:
    MegaTexture(const char * Filename);

    size_t VirtualSize() const  { return _virtualSize; } 
    size_t PageSize() const     { return _pageSize; }

    const PageEntry_t * Find(uint8_t Level, uint8_t X, uint8_t Y) const;
    const PageEntry_t * Find(uint32_t key);

    const std::vector<PageEntry_t> & table() const;

protected:
    std::vector<PageEntry_t>    _table;
    size_t                      _virtualSize;
    size_t                      _pageSize;
};

} // namespace mt

#endif