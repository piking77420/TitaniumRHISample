#ifndef TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H
#define TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H

#include <cstdint>
#include <string>
#include <vector>

#include <Assets/Asset.hpp>
#include <Rendering/StaticMesh.hpp>

namespace Ti::Plateform
{
    class App;
} // Ti::Plateform

namespace Ti::Assets
{
    class StaticMesh : public Asset
    {
    public:
        StaticMesh() = delete;
        ~StaticMesh() = default;
        StaticMesh(Plateform::App& App, std::string name, std::vector<Rendering::StaticMesh::Vertex> verticies,
                   std::vector<uint32_t> indicies);

        std::shared_ptr<Rendering::StaticMesh> getStaticMesh() const
        {
            return m_staticMesh;
        }

    private:
        std::vector<Rendering::StaticMesh::Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::shared_ptr<Rendering::StaticMesh> m_staticMesh;
        bool m_cpuAccesible = false;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_STATIC_MESH_H
