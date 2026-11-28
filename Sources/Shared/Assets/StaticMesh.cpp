#include <Assets/StaticMesh.hpp>

#include <utility>

namespace Ti::Assets
{
    StaticMesh::StaticMesh(Rendering::RhiThread& rhiThread, std::string name,
                           std::vector<Rendering::StaticMesh::Vertex> vertices, std::vector<uint32_t> indices)
        : Asset(std::move(name))
    {
        if (m_cpuAccesible)
        {
            m_vertices = vertices;
            m_indices = indices;
        }
        m_staticMesh.reset(
            new Rendering::StaticMesh(rhiThread, getName(), std::move(vertices), std::move(indices)));
    }

} // namespace Ti::Assets
