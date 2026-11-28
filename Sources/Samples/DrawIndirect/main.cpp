#include <iostream>
#include <print>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <format>
#include <string>
#include <string_view>
#include <span>
#include <optional>

#include <ImGuiIntegration/ImGuiIntegration.hpp>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include <Common/CameraExemple.hpp>
#include <Assets/Image.hpp>
#include <Assets/Texture.hpp>
#include <Rendering/Texture.hpp>
#include <Io/File.hpp>
#include <Core/Camera.hpp>
#include <Core/Log.hpp>
#include <Core/Math/EasingFunction.hpp>
#include <Core/Math/Projection.hpp>
#include <Core/Math/View.hpp>
#include <Common/CameraController.hpp>
#include <Matrix4x4.hpp>
#include <Vector3.hpp>
#include <Vector2.hpp>
#include <MatrixTransformation.hpp>
using namespace std::literals;
using namespace Ti::ShaderCompiler;
namespace fs = std::filesystem;

struct FrameConstantBuffer
{
    MathLib::Matrix4x4f wvp;
} frameConstantBuffer;

struct Vertex
{
    MathLib::Vector3f position;
    MathLib::Vector2f uv;
};

std::array<Vertex, 24> vertices = {{// Front (+Z)
                                    {MathLib::Vector3f{-0.5f, -0.5f, 0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, -0.5f, 0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, 0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{-0.5f, 0.5f, 0.5f}, MathLib::Vector2f{0.0f, 0.0f}},

                                    // Back (-Z)
                                    {MathLib::Vector3f{0.5f, -0.5f, -0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{-0.5f, -0.5f, -0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{-0.5f, 0.5f, -0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, -0.5f}, MathLib::Vector2f{0.0f, 0.0f}},

                                    // Left (-X)
                                    {MathLib::Vector3f{-0.5f, -0.5f, -0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{-0.5f, -0.5f, 0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{-0.5f, 0.5f, 0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{-0.5f, 0.5f, -0.5f}, MathLib::Vector2f{0.0f, 0.0f}},

                                    // Right (+X)
                                    {MathLib::Vector3f{0.5f, -0.5f, 0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, -0.5f, -0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, -0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, 0.5f}, MathLib::Vector2f{0.0f, 0.0f}},

                                    // Top (+Y)
                                    {MathLib::Vector3f{-0.5f, 0.5f, 0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, 0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, 0.5f, -0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{-0.5f, 0.5f, -0.5f}, MathLib::Vector2f{0.0f, 0.0f}},

                                    // Bottom (-Y)
                                    {MathLib::Vector3f{-0.5f, -0.5f, -0.5f}, MathLib::Vector2f{0.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, -0.5f, -0.5f}, MathLib::Vector2f{1.0f, 1.0f}},
                                    {MathLib::Vector3f{0.5f, -0.5f, 0.5f}, MathLib::Vector2f{1.0f, 0.0f}},
                                    {MathLib::Vector3f{-0.5f, -0.5f, 0.5f}, MathLib::Vector2f{0.0f, 0.0f}}}};

std::array<uint16_t, 36> indices = {{// Front (+Z)
                                     0, 1, 2, 0, 2, 3,

                                     // Back (-Z)
                                     4, 5, 6, 4, 6, 7,

                                     // Left (-X)
                                     8, 9, 10, 8, 10, 11,

                                     // Right (+X)
                                     12, 13, 14, 12, 14, 15,

                                     // Top (+Y)
                                     16, 17, 18, 16, 18, 19,

                                     // Bottom (-Y)
                                     20, 21, 22, 20, 22, 23}};

class DrawIndirect : public Ti::Common::CameraExemple
{
public:
    DrawIndirect() = delete;
    virtual ~DrawIndirect() = default;

    DrawIndirect(std::string name)
        : Ti::Common::CameraExemple(std::move(name))
        , m_depthTexture(getDevice())
        , m_renderTargets(getDevice())
        , m_renderPassDescriptor(getDevice())
        , m_texture(getDevice())
        , m_vertexBuffer(getDevice())
        , m_indexBuffer(getDevice())
        , m_rootConstantBuffer(getDevice())
        , m_cubeSet(getDevice())
        , m_cubePipeline(getDevice())
    {
        setupSceneRenderPass();
        setupTexture();
        setupVertexDescription();
        setupBuffer();
        setupDescriptorSet();
        setupPipeline();
    }

protected:
    TiRHI::Texture m_depthTexture;

    TiRHI::RenderTargets m_renderTargets;

    TiRHI::RenderPassDescriptor m_renderPassDescriptor;

    TiRHI::Texture m_texture;

    TiRHI::Buffer m_vertexBuffer;

    TiRHI::Buffer m_indexBuffer;

    TiRHI::Buffer m_rootConstantBuffer;

    TiRHI::DescriptorSet m_cubeSet;

    std::array<TiRHI::VertexBindingDescription, 1> m_vertexBindingDescription;

    std::array<TiRHI::VertexInputAttributeDescription, 2> m_vertexInputAttributeDescription;

    TiRHI::GraphicPipeline m_cubePipeline;

    MathLib::Vector3f m_cubePosition = MathLib::Vector3f::zero();

    MathLib::Vector3f m_cubeRotation = MathLib::Vector3f::zero();

    MathLib::Vector3f m_cubeScaling = MathLib::Vector3f(1.f, 1.f, 1.f);

    void onResizeWindow() final
    {
        CameraExemple::onResizeWindow();
        setupSceneRenderPass();
    }

    void onUpdate() final
    {
        CameraExemple::onUpdate();

        static std::string windowName = [&]()
        { return std::string(getName()) + " " + std::string(TiRHI::toString(getRHI().getApi())); }();
        ImGui::Begin(windowName.c_str());
        ImGui::DragFloat3("Postion", reinterpret_cast<float*>(&m_cubePosition));
        ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&m_cubeRotation));
        ImGui::DragFloat3("Scaling", reinterpret_cast<float*>(&m_cubeScaling));
        ImGui::End();
    }

    void onRender()
    {
        const float aspect = static_cast<float>(getWindow().getWidth()) / static_cast<float>(getWindow().getHeight());
        const MathLib::Matrix4x4f projection = Ti::Core::Math::perspectiveRH(1.39626f, aspect, 0.1f, 1000.0f);
        assert(projection == m_camera.getProjection());
        const MathLib::Matrix4x4f model =
            MathLib::trs<float>(m_cubePosition, m_cubeRotation * MathLib::DegToRadf, m_cubeScaling);
        const MathLib::Matrix4x4f wvp = projection * m_camera.getView() * model;

        if (std::byte* ptr = m_rootConstantBuffer.beginDynamicUpdate())
        {
            MathLib::Matrix4x4f* m = reinterpret_cast<MathLib::Matrix4x4f*>(ptr);
            *m = wvp;

            m_rootConstantBuffer.endDynamicUpdate();
        }
    }

    void onRecord(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
        // nothing
        {
            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(getSwapChain().getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;

            TiRHI::ClearValueDepthStencil clearColorStencil;
            clearColorStencil.depth = 1.0f;

            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor).setClearDepthStencil(clearColorStencil);

            cmd.transitionResource(m_sceneViewTexture, TiRHI::ResourceState::RenderTarget);
            cmd.transitionResource(m_depthTexture, TiRHI::ResourceState::DepthWrite);

            TiRHI::DebugScope _(cmd, "Scene Pass"sv, std::array{0.1f, 1.0f, 0.0f, 1.0f});
            cmd.beginRenderPass(renderPass, m_renderTargets);
            TiRHI::Viewport viewPort;
            viewPort.setPosition({0, 0}).setExtend(TiRHI::Extend2DF{static_cast<float>(m_renderTargets.getWidth()),
                                                                    static_cast<float>(m_renderTargets.getHeight())});
            cmd.setViewPort(viewPort);

            TiRHI::Rect2D scissors;
            scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
            cmd.setScissors(scissors);
            cmd.bindPipeline(m_cubePipeline);
            cmd.bindDescriptorSet(m_cubeSet, 0);
            cmd.bindVertexBuffer(0, 0, m_vertexBuffer);
            cmd.bindIndexBuffer(m_indexBuffer, 0, TiRHI::IndexType::UInt16);

            cmd.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            cmd.endRenderPass();
        }
        cmd.transitionResource(m_sceneViewTexture, TiRHI::ResourceState::ShaderResource);
    }

    void setupSceneRenderPass()
    {
        m_depthTexture = TiRHI::Texture{getDevice()};
        if (!m_depthTexture.setName("SceneDepth")
                 .setFlags(TiRHI::TextureFlags::RenderTarget | TiRHI::TextureFlags::ShaderResource)
                 .setType(TiRHI::TextureType::Texture2D)
                 .setWidth(static_cast<uint32_t>(getSwapChain().getWidth()))
                 .setHeight(static_cast<uint32_t>(getSwapChain().getHeight()))
                 .setFormat(TiRHI::Format::D32FloatS8UInt)
                 .build())
        {
            TI_LOG_FATAL(Rendering, "Failed to build {}", m_depthTexture.getName());
        }

        TiRHI::AttachmentDescriptor colorDescriptor{};
        m_sceneViewTexture.set(colorDescriptor);
        colorDescriptor.setLoadOp(TiRHI::LoadOp::Clear).setStoreOp(TiRHI::StoreOp::Store);
        TiRHI::AttachmentDescriptor depthDescriptor{};
        m_depthTexture.set(depthDescriptor);
        depthDescriptor.setLoadOp(TiRHI::LoadOp::Clear).setStoreOp(TiRHI::StoreOp::Store);

        m_renderPassDescriptor = TiRHI::RenderPassDescriptor{getDevice()};
        if (!m_renderPassDescriptor.setName("Scene RenderPassDescriptor")
                 .setColorAttachments(std::array{colorDescriptor})
                 .setDepthAttachement(depthDescriptor)
                 .build())
        {
            TI_LOG_FATAL(Rendering, "Failed to build {}", m_renderPassDescriptor.getName());
        }

        m_renderTargets = TiRHI::RenderTargets{getDevice()};
        if (!m_renderTargets.bindColorAttachement(m_sceneViewTexture, 0)
                 .bindDepthAttachement(m_depthTexture, 1)
                 .setWidth(m_sceneViewTexture.getWidth())
                 .setHeight(m_sceneViewTexture.getHeight())
                 .setRenderPassDescriptor(&m_renderPassDescriptor)
                 .setName("SceneViewRenderTarget")
                 .build())
        {
            TI_LOG_FATAL(Rendering, "Failed to build {}", m_renderTargets.getName());
        }
    }

    void setupTexture()
    {
        Ti::Assets::Image image(fs::path(TITANIUM_RESOURCE_DIR) / "DiamondOre.png");

        if (!image)
        {
            TI_LOG_FATAL(Core, "Failed to load image");
        }

        {
            const bool result =
                m_texture.setName("Sampled Texture")
                    .setWidth(image.getWidth())
                    .setHeight(image.getHeight())
                    .setFlags(TiRHI::TextureFlags::CopyDestination | TiRHI::TextureFlags::ShaderResource)
                    .setFormat(TiRHI::Format::R8G8B8A8UNorm)
                    .setType(TiRHI::TextureType::Texture2D)
                    .build();

            if (!result)
            {
                TI_LOG_FATAL(Core, "Failed to build texure");
            }
        }

        pushRenderCommand(
            [this, size = image.getTotalSize(), data = std::move(image).acquireData()](TiRHI::CommandList& command)
            {
                command.upload2d(m_texture, std::as_bytes(std::span(data.get(), size)));
                command.transitionResource(m_texture, TiRHI::ResourceState::ShaderResource);
            });
    }

    void setupVertexDescription()
    {
        m_vertexBindingDescription[0].binding = 0;
        m_vertexBindingDescription[0].stride = sizeof(Vertex);
        m_vertexBindingDescription[0].vertexInputRate = TiRHI::VertexInputRate::Vertex;

        // Position
        m_vertexInputAttributeDescription[0].binding = 0;
        m_vertexInputAttributeDescription[0].format = TiRHI::Format::R32G32B32Float;
        m_vertexInputAttributeDescription[0].offset = offsetof(Vertex, position);
        m_vertexInputAttributeDescription[0].location = 0;

        // UV
        m_vertexInputAttributeDescription[1].binding = 0;
        m_vertexInputAttributeDescription[1].format = TiRHI::Format::R32G32Float;
        m_vertexInputAttributeDescription[1].offset = offsetof(Vertex, uv);
        m_vertexInputAttributeDescription[1].location = 1;
    }

    void setupPipeline()
    {
        m_cubePipeline = std::move(compilePsoVertexFrag("cube.hlsl"sv, L"cube.vert.spriv", L"cube.pixel.spriv"));

        m_cubePipeline.setName("Cube Pipeline")
            .setDescriptorSetLayoutEntries(
                std::array{TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_cubeSet.getBindingsLayout(), 0}})
            .setVertexBindingDescription(m_vertexBindingDescription)
            .setVertexInputAttributeDescription(m_vertexInputAttributeDescription)
            .setRenderPassDescritptor(&m_renderPassDescriptor)
            .setDepthCompareOp(TiRHI::CompareOp::Less);

        if (!m_cubePipeline.build())
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline {}", m_cubePipeline.getName());
        }
    }

    void setupDescriptorSet()
    {
        const bool success =
            m_cubeSet.setName("Cube Set")
                .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &m_texture, &m_samplerNearestRepeat, 0)
                .bindConstantBuffer(TiRHI::ShaderStageFlags::Vertex, &m_rootConstantBuffer, 1)
                .build();

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build descriptor set");
        }
    }

    void setupBuffer()
    {
        bool success = m_vertexBuffer.setName("VertexBuffer")
                           .setMemoryType(TiRHI::BufferMemoryType::Default)
                           .setUpdateMode(TiRHI::BufferUpdateMode::Static)
                           .setUsage(TiRHI::BufferUsage::Vertex | TiRHI::BufferUsage::CopyDestination)
                           .setSize(sizeof(vertices))
                           .build();
        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build VertexBuffer");
            return;
        }

        pushRenderCommand(
            [this, verticiesToUpload = vertices](TiRHI::CommandList& command)
            {
                command.uploadBuffer(m_vertexBuffer, 0,
                                     std::as_bytes(std::span(verticiesToUpload.data(), verticiesToUpload.size())));
                command.transitionResource(m_vertexBuffer, TiRHI::ResourceState::VertexBuffer);
            });

        success = m_indexBuffer.setName("IndexBuffer")
                      .setMemoryType(TiRHI::BufferMemoryType::Default)
                      .setUpdateMode(TiRHI::BufferUpdateMode::Static)
                      .setUsage(TiRHI::BufferUsage::Index | TiRHI::BufferUsage::CopyDestination)
                      .setSize(sizeof(indices))
                      .build();

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build Index Buffer");
            return;
        }

        pushRenderCommand(
            [this, indicesToUpload = indices](TiRHI::CommandList& command)
            {
                command.transitionResource(m_indexBuffer, TiRHI::ResourceState::CopyDestination);
                command.uploadBuffer(m_indexBuffer, 0,
                                     std::as_bytes(std::span(indicesToUpload.data(), indicesToUpload.size())));
                command.transitionResource(m_indexBuffer, TiRHI::ResourceState::IndexBuffer);
            });

        success = m_rootConstantBuffer.setName("ConstantBuffer")
                      .setMemoryType(TiRHI::BufferMemoryType::Upload)
                      .setUpdateMode(TiRHI::BufferUpdateMode::Dynamic)
                      .setUsage(TiRHI::BufferUsage::Constant)
                      .setSize(getDevice().alignConstantBuffer(sizeof(frameConstantBuffer)))
                      .build();

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build ConstantBuffer Buffer");
            return;
        }
    }
};

int main()
{
    DrawIndirect drawIndirect("DrawIndirect");
    drawIndirect.run();
    return 0;
}
