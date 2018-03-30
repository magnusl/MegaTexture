#include "megatexture.h"
#include <stdio.h>

namespace mt {

static bool ReadFileHeader(FILE * file, FileHeader_t & hdr)
{
    if (!file) {
        return false;
    }
    if (fread(&hdr, sizeof(FileHeader_t), 1, file) != 1) {
        return false;
    }
    if (memcmp(hdr.Id, "_MT_", sizeof(hdr.Id))) {
        return false;
    }
    return true;
}

static bool ReadPageEntries(FILE * file, size_t Count, std::vector<PageEntry_t> & Pages)
{
    Pages.resize(Count);
    if (fread(&Pages[0], sizeof(PageEntry_t), Count, file) != Count) {
        return false;
    }   
    return true;
}

MegaTexture::MegaTexture(const char * Filename)
{
    FILE * file = fopen(Filename, "rb");

    FileHeader_t hdr;
    if (!ReadFileHeader(file, hdr)) {
        fclose(file);
        throw std::runtime_error("Failed to read MegaTexture file header.");
    }

    // the number of pages are located at the end of the file
    if (_fseeki64(file, -4, SEEK_END)) {
        fclose(file);
        throw std::runtime_error("Failed to seek.");
    }

    uint32_t count;
    if (fread(&count, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        throw std::runtime_error("Failed to read page count.");
    }

    if (_fseeki64(file, -static_cast<long>((sizeof(uint32_t) + (count * sizeof(PageEntry_t)))), SEEK_END)) {
        fclose(file);
        throw std::runtime_error("Failed to seek.");
    }

    if (!ReadPageEntries(file, count, _table)) {
        fclose(file);
        throw std::runtime_error("Failed to read page entries.");
    }
    
    fclose(file);

    _pageSize       = hdr.PageSize;
    _virtualSize    = hdr.VirtualSize;
}

const PageEntry_t * MegaTexture::Find(uint8_t Level, uint8_t X, uint8_t Y) const
{
    uint32_t key =
        (static_cast<uint32_t>(Level) << 16) |
        (static_cast<uint32_t>(X) << 8) |
        static_cast<uint32_t>(Y);

    return nullptr;
}

const PageEntry_t * MegaTexture::Find(uint32_t key)
{
    for(size_t i = 0, count = _table.size(); i < count; ++i) {
        if (_table[i].PageId == key) {
            return &_table[i];
        }
    }
    return nullptr;
}

} // namespace mt