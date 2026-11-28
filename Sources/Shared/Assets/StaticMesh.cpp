#include <Assets/StaticMesh.hpp>

#include <utility>

namespace Ti::Assets
{
    StaticMesh::StaticMesh(Plateform::App& App, std::string name, std::vector<Rendering::StaticMesh::Vertex> verticies,
                           std::vector<uint32_t> indicies)
        : Asset(std::move(name))
    {
        if (m_cpuAccesible)
        {
            m_vertices = verticies;
            m_indices = indicies;
        }
        m_staticMesh.reset(new Rendering::StaticMesh(App, getName(), std::move(verticies), std::move(indicies)));
    }

} // namespace Ti::Assets
