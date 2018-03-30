#include "scene.h"
#include <stdexcept>
#include <cstdio>
#include <vector>

namespace mt {

Scene::Scene(const char * Filename)
{
    if (!Filename) {
        throw std::runtime_error("Invalid filename");
    }

    FILE * file = fopen(Filename, "rb");
    if (!file) {
        throw std::runtime_error("Failed to open scene file.");
    }

    struct Header {
        char        id[4];
        uint32_t    NumVerticies;
    } hdr;

    if (fread(&hdr, sizeof(hdr), 1, file) != 1) {
        fclose(file);
        throw std::runtime_error("Failed to read file header.");
    }

    if (memcmp(hdr.id, "_MS_", 4)) {
        fclose(file);
        throw std::runtime_error("Invalid file signature");
    }

    std::vector<float> data(hdr.NumVerticies * 5);
    if (fread(&data[0], sizeof(float) * 5, hdr.NumVerticies, file) != hdr.NumVerticies) {
        fclose(file);
        throw std::runtime_error("Failed to load vertex data.");
    }

    fclose(file);

    _buffer = std::make_unique<ogl::VertexBuffer>(
        &data[0], data.size() * sizeof(float), ogl::VertexBuffer::V3_T2);
}

Scene::~Scene()
{
}


} // namespace mt
