#ifndef TITANIUM_SAMPLE_ASSETS_MESH_SECTION_H
#define TITANIUM_SAMPLE_ASSETS_MESH_SECTION_H

#include <cstdint>

namespace Ti::Assets
{
    struct MeshSection
    {
        uint32_t firstIndex = 0;
        uint32_t indexCount = 0;
        int32_t vertexOffset = 0;
        uint32_t materialIndex = 0;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_MESH_SECTION_H
