#include <Assets/StaticMesh.hpp>

#include <utility>

namespace Ti::Assets
{
    StaticMesh::StaticMesh(Rendering::RhiThread& rhiThread, std::string name,
                           std::vector<Rendering::StaticMesh::Vertex> vertices, std::vector<uint32_t> indices,
                           std::vector<MeshSection> sections)
        : Asset(std::move(name))
        , m_sections(std::move(sections))
    {
        if (m_cpuAccesible)
        {
            m_vertices = vertices;
            m_indices = indices;
            m_sections = sections;
        }
        m_staticMesh.reset(new Rendering::StaticMesh(rhiThread, getName(), std::move(vertices), std::move(indices)));
    }

} // namespace Ti::Assets
