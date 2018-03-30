#include "pagetable.h"
#include "oglbufferedtexture.h"

namespace mt {

PageTable::PageTable(size_t TableSize, size_t NumLevels) : _tableSize(TableSize)
{
    size_t size = TableSize;
    _pageTables.resize(NumLevels);
    for(size_t i = 0; i < NumLevels; ++i) {
        std::vector<PageTableEntry_t> & entries = _pageTables[i].PageEntries;
        entries.resize(size * size);
        size >>= 1;
    }
}

inline size_t GetPageIndex(size_t TableSize, size_t MipLevel, uint16_t X, uint16_t Y)
{
    size_t LevelSize = TableSize >> MipLevel;
    return (LevelSize * Y) + X;
}

inline uint16_t GetPhysicalAddress(uint8_t X, uint8_t Y)
{
    return (static_cast<uint16_t>(X) << 8) | Y;
}

bool PageTable::Map(uint32_t Key, uint16_t PhysicalAddr)
{
    return Map((Key >> 16) & 0xff, (Key >> 8) & 0xff, Key & 0xff, PhysicalAddr);
}

bool PageTable::Map(unsigned MipLevel, uint16_t PageX, uint16_t PageY, uint16_t PhysicalAddr)
{
    size_t LevelSize    = _tableSize >> MipLevel;

    // indicies into the physical texture, map to [0..1] range
    size_t x = (PhysicalAddr / 32) * 8;
    size_t y = (PhysicalAddr % 32) * 8;

    //uint8_t physX     = PhysicalAddr >> 8;
    //uint8_t physY     = PhysicalAddr & 0xff;

    // index into the page table
    size_t pageIndex    = GetPageIndex(_tableSize, MipLevel, PageX, PageY);
    uint8_t size        = 1;

    // write the page entry
    PageTableEntry_t & pe = _pageTables[MipLevel].PageEntries[pageIndex];
    pe.miplevel = MipLevel;
    pe.x        = x;
    pe.y        = y;

    for(int i = static_cast<int>(MipLevel) - 1; i >= 0; i--) {
        LevelSize       = _tableSize >> i;
        Table & table   = _pageTables[i];
        // calculate position and size at this mip-level
        PageX   <<= 1;
        PageY   <<= 1;
        size    <<= 1;

        bool cnt = false;
        // access the page entries and see if thet need to be updated
        for(size_t _y = PageY; _y < (PageY + size); ++_y) {
            for(size_t _x = PageX; _x < (PageX + size); ++_x) {

                PageTableEntry_t & ref =
                    table.PageEntries[ GetPageIndex(_tableSize, i, _x, _y)];

                if (ref.miplevel == 0xff) { //< entry is invalid
                    ref = pe;
                    cnt = true;
                }
            }
        }
        if (!cnt) {
            return true;
        }
    }
    return true;
}

bool PageTable::Unmap(uint32_t Key)
{
    return Unmap((Key >> 16) & 0xff, (Key >> 8) & 0xff, Key & 0xff);
}

bool PageTable::Unmap(unsigned MipLevel, uint16_t PageX, uint16_t PageY)
{
    // Cannot unmap a entry in the lowest mip level, since we don't have
    // anything to replace it with
    if (MipLevel >= _pageTables.size()) {
        return false;
    }

    size_t pageIndex        = GetPageIndex(_tableSize, MipLevel, PageX, PageY);
    PageTableEntry_t & pe   = _pageTables[MipLevel].PageEntries[pageIndex];
    //
    // Mark the entry as invalid, and then map the underlying page, which
    // will then propagate the changes upwards.
    //
    pe.miplevel = 0xff;
    size_t index = GetPageIndex(_tableSize, MipLevel + 1, PageX / 2, PageY / 2);
    const PageTableEntry_t & underlying = _pageTables[MipLevel + 1].PageEntries[index];
    return Map(MipLevel + 1, PageX / 2, PageY / 2, GetPhysicalAddress(underlying .x, underlying .y));
}

void PageTable::Commit(ogl::BufferedTexture & Texture)
{
    size_t w = this->_tableSize;
    size_t h = this->_tableSize;

    Texture.Update(0, 0, w, h, (uint8_t *)&_pageTables[0].PageEntries[0], 0);
    Texture.Commit(nullptr);
}

} // namespace mt
