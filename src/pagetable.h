#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>
#include <vector>

namespace ogl {

class BufferedTexture;

}

namespace mt {

struct PageTableEntry_t
{
    PageTableEntry_t() : miplevel(0xff)
    {
    }

    uint8_t     x;
    uint8_t     miplevel;
    uint8_t     y;
};

struct Table {
    std::vector<PageTableEntry_t>   PageEntries;
};

class PageTable
{
public:
    PageTable(size_t TableSize, size_t NumLevels);
    ~PageTable()
    {
    }

    bool Map(uint32_t Key, uint16_t PhysicalAddr);
    bool Map(unsigned MipLevel, uint16_t PageX, uint16_t PageY, uint16_t PhysicalAddr);
    bool Unmap(unsigned MipLevel, uint16_t PageX, uint16_t PageY);
    bool Unmap(uint32_t Key);

    void Commit(ogl::BufferedTexture &);

protected:
    std::vector<Table>  _pageTables;
    size_t              _tableSize;
};

} // namespace mt

#endif