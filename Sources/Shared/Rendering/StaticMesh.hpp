#ifndef TITANIUM_SAMPLE_RENDERING_STATIC_MESH_H
#define TITANIUM_SAMPLE_RENDERING_STATIC_MESH_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <Titanium/TitaniumHeader.hpp>
#include <Rendering/GpuResource.hpp>
#include <Vector3.hpp>
#include <Vector2.hpp>
#include <Matrix4x4.hpp>

namespace Ti::Plateform
{
    class App;
} // Ti::Plateform

namespace Ti::Rendering
{

    class StaticMesh : public GpuResource
    {
    public:
        struct Vertex
        {
            MathLib::Vector3f position;
            MathLib::Vector3f normal;
            MathLib::Vector2f uv;
        };

        StaticMesh() = default;
        // TODO PUT RHI THREAD IN PARAM AND PER REF IN ORDER TO DESTROY
        explicit StaticMesh(Ti::Plateform::App& app, std::string_view staticMeshName, std::vector<Vertex> verticies,
                            std::vector<uint32_t> indicies);
        ~StaticMesh() override = default;

        bool isValid() const;

        TiRHI::Buffer* getVertexBuffer() noexcept
        {
            return m_staticMeshResource ? &m_staticMeshResource->vertexBuffer : nullptr;
        }

        TiRHI::Buffer* getIndexBuffer() noexcept
        {
            return m_staticMeshResource ? &m_staticMeshResource->indexBuffer : nullptr;
        }

        std::optional<uint32_t> getIndexCount() const noexcept
        {
            if (m_staticMeshResource)
                return m_staticMeshResource->indexCount;

            return std::nullopt;
        }

        std::optional<TiRHI::IndexType> getIndexType() const noexcept
        {
            if (m_staticMeshResource)
                return m_staticMeshResource->indexType;

            return std::nullopt;
        }

        static std::array<TiRHI::VertexBindingDescription, 1> getVertexBindingDescription()
        {
            return std::array<TiRHI::VertexBindingDescription, 1>{TiRHI::VertexBindingDescription{
                .binding = 0, .stride = sizeof(Vertex), .vertexInputRate = TiRHI::VertexInputRate::Vertex}};
        }

        static std::array<TiRHI::VertexInputAttributeDescription, 3> getVertexInputAttributeDescription()
        {
            return {{
                {0, 0, TiRHI::Format::R32G32B32Float, static_cast<uint32_t>(offsetof(Vertex, position))},
                {0, 1, TiRHI::Format::R32G32B32Float, static_cast<uint32_t>(offsetof(Vertex, normal))},
                {0, 2, TiRHI::Format::R32G32Float, static_cast<uint32_t>(offsetof(Vertex, uv))},
            }};
        }

        struct DrawCommand
        {
            TiRHI::Buffer* vertexBuffer = nullptr;
            TiRHI::Buffer* indexBuffer = nullptr;
            uint32_t indexCount = 0;
            TiRHI::IndexType indexType{};
        };

        std::optional<DrawCommand> toDrawCommand()
        {
            if (!isValid())
                return std::nullopt;

            return DrawCommand{&m_staticMeshResource->vertexBuffer, &m_staticMeshResource->indexBuffer,
                               m_staticMeshResource->indexCount, m_staticMeshResource->indexType};
        }

    private:
        struct StaticMeshResource
        {
            TiRHI::Buffer vertexBuffer;
            TiRHI::Buffer indexBuffer;
            uint32_t indexCount = 0;
            TiRHI::IndexType indexType{};

            explicit StaticMeshResource(TiRHI::Device& device, uint32_t newIndexCount, TiRHI::IndexType newType)
                : vertexBuffer(device)
                , indexBuffer(device)
                , indexCount(newIndexCount)
                , indexType(newType)
            {
            }
        };
        std::optional<StaticMeshResource> m_staticMeshResource;
    };

} // namespace Ti::Rendering

#endif // TITANIUM_SAMPLE_RENDERING_STATIC_MESH_H
