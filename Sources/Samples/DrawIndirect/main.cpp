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
#include <Assets/ModelImporter.hpp>
#include <Assets/StaticMesh.hpp>
#include <Rendering/Texture.hpp>
#include <Io/File.hpp>
#include <Core/Camera.hpp>
#include <Core/Log.hpp>
#include <Core/Math/EasingFunction.hpp>
#include <Core/Math/Projection.hpp>
#include <Core/Math/View.hpp>
#include <Common/CameraController.hpp>
#include <Rendering/StaticMesh.hpp>
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
        , m_rootConstantBuffer(getDevice())
        , m_meshSet(getDevice())
        , m_meshPipeline(getDevice())
    {
        const fs::path meshPath = fs::path(TITANIUM_RESOURCE_DIR) / "stanford_dragon_vrip.glb";
        m_staticMesh = Ti::Assets::ModelImporter(meshPath).makeStaticMesh(*this);
        if (!m_staticMesh)
        {
            TI_LOG_FATAL(Plateform, "Failed to load mesh {}", meshPath.generic_string());
        }

        setupSceneRenderPass();
        setupTexture();
        setupBuffer();
        setupDescriptorSet();
        setupPipeline();
    }

protected:
    Ti::Assets::AssetPtr<Ti::Assets::StaticMesh> m_staticMesh;

    TiRHI::Texture m_depthTexture;

    TiRHI::RenderTargets m_renderTargets;

    TiRHI::RenderPassDescriptor m_renderPassDescriptor;

    TiRHI::Texture m_texture;

    TiRHI::Buffer m_rootConstantBuffer;

    TiRHI::DescriptorSet m_meshSet;

    TiRHI::GraphicPipeline m_meshPipeline;

    MathLib::Vector3f m_meshPosition = MathLib::Vector3f::zero();

    MathLib::Vector3f m_meshRotation = MathLib::Vector3f::zero();

    MathLib::Vector3f m_meshScaling = MathLib::Vector3f(1.f, 1.f, 1.f);

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
        ImGui::DragFloat3("Postion", reinterpret_cast<float*>(&m_meshPosition));
        ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&m_meshRotation));
        ImGui::DragFloat3("Scaling", reinterpret_cast<float*>(&m_meshScaling));
        ImGui::End();
    }

    void onRender()
    {
        const float aspect = static_cast<float>(getWindow().getWidth()) / static_cast<float>(getWindow().getHeight());
        const MathLib::Matrix4x4f projection = Ti::Core::Math::perspectiveRH(1.39626f, aspect, 0.1f, 1000.0f);
        assert(projection == m_camera.getProjection());
        const MathLib::Matrix4x4f model =
            MathLib::trs<float>(m_meshPosition, m_meshRotation * MathLib::DegToRadf, m_meshScaling);
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
            if (std::shared_ptr<Ti::Rendering::StaticMesh> renderStaticMesh = m_staticMesh->getStaticMesh())
            {
                if (std::optional<Ti::Rendering::StaticMesh::DrawCommand> drawCommand =
                        renderStaticMesh->toDrawCommand())
                {
                    cmd.bindPipeline(m_meshPipeline);
                    cmd.bindDescriptorSet(m_meshSet, 0);
                    cmd.bindVertexBuffer(0, 0, *drawCommand->vertexBuffer);
                    cmd.bindIndexBuffer(*drawCommand->indexBuffer, 0, drawCommand->indexType);

                    cmd.drawIndexed(static_cast<uint32_t>(drawCommand->indexCount), 1, 0, 0, 0);
                }
            }

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

        pushRhiCommand(
            [this, size = image.getTotalSize(), data = std::move(image).acquireData()](TiRHI::Device& device,
                                                                                       TiRHI::CommandList& command)
            {
                command.upload2d(m_texture, std::as_bytes(std::span(data.get(), size)));
                command.transitionResource(m_texture, TiRHI::ResourceState::ShaderResource);
            });
    }

    void setupPipeline()
    {
        m_meshPipeline = std::move(compilePsoVertexFrag("cube.hlsl"sv, L"cube.vert.spriv", L"cube.pixel.spriv"));

        m_meshPipeline.setName("Mesh Pipeline")
            .setDescriptorSetLayoutEntries(
                std::array{TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_meshSet.getBindingsLayout(), 0}})
            .setVertexBindingDescription(Ti::Rendering::StaticMesh::getVertexBindingDescription())
            .setVertexInputAttributeDescription(Ti::Rendering::StaticMesh::getVertexInputAttributeDescription())
            .setRenderPassDescritptor(&m_renderPassDescriptor)
            .setDepthCompareOp(TiRHI::CompareOp::Less);

        if (!m_meshPipeline.build())
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline {}", m_meshPipeline.getName());
        }
    }

    void setupDescriptorSet()
    {
        const bool success =
            m_meshSet.setName("Mesh Set")
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
        const bool success = m_rootConstantBuffer.setName("ConstantBuffer")
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
