#ifndef TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H
#define TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H

#include <cstdint>
#include <string>
#include <vector>

#include <Assets/Asset.hpp>
#include <Assets/MeshSection.hpp>
#include <Rendering/StaticMesh.hpp>
#include <Assets/MeshSection.hpp>

namespace Ti::Assets
{
    class StaticMesh : public Asset
    {
    public:
        StaticMesh() = delete;
        ~StaticMesh() = default;
        StaticMesh(Rendering::RhiThread& rhiThread, std::string name,
                   std::vector<Rendering::StaticMesh::Vertex> vertices, std::vector<uint32_t> indices,
                   std::vector<MeshSection> sections);

        std::shared_ptr<Rendering::StaticMesh> getStaticMesh() const
        {
            return m_staticMesh;
        }

        const std::vector<MeshSection>& getSections() const noexcept
        {
            return m_sections;
        }

    private:
        std::vector<Rendering::StaticMesh::Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<MeshSection> m_sections;
        std::shared_ptr<Rendering::StaticMesh> m_staticMesh;
        bool m_cpuAccesible = false;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H
