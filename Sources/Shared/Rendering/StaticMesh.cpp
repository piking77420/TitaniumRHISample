#include <Rendering/StaticMesh.hpp>
#include <Rendering/RhiThread.hpp>

#include <limits>
#include <span>
#include <string>
#include <utility>
#include <Core/Log.hpp>

namespace Ti::Rendering
{
    StaticMesh::StaticMesh(RhiThread& rhiThread, std::string_view staticMeshName, std::vector<Vertex> verticies,
                           std::vector<uint32_t> indices)

    {
        if (verticies.empty() || indices.empty() || indices.size() > std::numeric_limits<uint32_t>::max())
        {
            TI_LOG_ERROR(Rendering, "Empty Static Mesh");
            return;
        }

        for (const uint32_t index : indices)
        {
            if (index >= verticies.size())
            {
                TI_LOG_ERROR(Rendering, "Static mesh '{}' contains an out-of-range index", staticMeshName);
                return;
            }
        }

        rhiThread.pushOnRecordCommand(
            [this, vertices = std::move(verticies), indices = std::move(indices),
             name = std::string(staticMeshName)](TiRHI::Device& device, TiRHI::CommandList& cmd) mutable
            {
                // TODO HAD PROFILER SCOPE

                m_staticMeshResource.emplace(device, static_cast<uint32_t>(indices.size()), TiRHI::IndexType::UInt32);

                const auto vertexData = std::as_bytes(std::span(vertices));
                const auto indexData = std::as_bytes(std::span(indices));

                if (!m_staticMeshResource->vertexBuffer.setName(name + " VertexBuffer")
                         .setMemoryType(TiRHI::BufferMemoryType::Default)
                         .setUpdateMode(TiRHI::BufferUpdateMode::Static)
                         .setUsage(TiRHI::BufferUsage::Vertex | TiRHI::BufferUsage::CopyDestination)
                         .setSize(vertexData.size())
                         .build())
                {
                    TI_LOG_ERROR(Rendering, "Failed to build vertex buffer for static mesh '{}'", name);
                    return;
                }

                if (!m_staticMeshResource->indexBuffer.setName(name + " IndexBuffer")
                         .setMemoryType(TiRHI::BufferMemoryType::Default)
                         .setUpdateMode(TiRHI::BufferUpdateMode::Static)
                         .setUsage(TiRHI::BufferUsage::Index | TiRHI::BufferUsage::CopyDestination)
                         .setSize(indexData.size())
                         .build())
                {
                    TI_LOG_ERROR(Rendering, "Failed to build index buffer for static mesh '{}'", name);
                    return;
                }

                // TODO BATCH BARRIER
                cmd.transitionResource(m_staticMeshResource->vertexBuffer, TiRHI::ResourceState::CopyDestination);
                cmd.uploadBuffer(m_staticMeshResource->vertexBuffer, 0, std::as_bytes(std::span(vertices)));
                cmd.transitionResource(m_staticMeshResource->vertexBuffer, TiRHI::ResourceState::VertexBuffer);

                cmd.transitionResource(m_staticMeshResource->indexBuffer, TiRHI::ResourceState::CopyDestination);
                cmd.uploadBuffer(m_staticMeshResource->indexBuffer, 0, std::as_bytes(std::span(indices)));
                cmd.transitionResource(m_staticMeshResource->indexBuffer, TiRHI::ResourceState::IndexBuffer);
            });
    }

    bool StaticMesh::isValid() const
    {
        return m_staticMeshResource && m_staticMeshResource->vertexBuffer && m_staticMeshResource->indexBuffer &&
               m_staticMeshResource->indexCount > 0;
    }

} // namespace Ti::Rendering
