#include <iostream>
#include <print>
#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <format>
#include <string>
#include <string_view>
#include <span>

#include <ImGuiIntegration/ImGuiIntegration.hpp>
#include <imgui.h>

#include <Common/Exemple.hpp>
#include <Assets/Image.hpp>
#include <Assets/Texture.hpp>
#include <Rendering/Texture.hpp>
#include <Io/File.hpp>
#include <Core/Log.hpp>
#include <Core/Math/Projection.hpp>
#include <Core/Math/View.hpp>
using namespace std::literals;
using namespace Ti::ShaderCompiler;
namespace fs = std::filesystem;
#undef min
#undef max
#include <Matrix4x4.hpp>
#include <Vector3.hpp>
#include <Vector2.hpp>
#include <MatrixTransformation.hpp>

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

class CubeRotated : public Ti::Common::Exemple
{
public:
    CubeRotated() = delete;
    virtual ~CubeRotated() = default;

    CubeRotated(std::string name)
        : Ti::Common::Exemple(std::move(name))
        , m_texture(getRHI())
        , m_sampler(getRHI())
        , m_pipeline(getRHI())
        , m_vertexBuffer(getRHI())
        , m_indexBuffer(getRHI())
        , m_rootConstantBuffer(getRHI())
        , m_set(getRHI())
    {
        setupTexture();
        setupSampler();
        setupVertexDescription();
        setupBuffer();
        setupDescriptorSet();
        setupPipeline();
    }

protected:
    TiRHI::Texture m_texture;

    TiRHI::Sampler m_sampler;

    TiRHI::GraphicPipeline m_pipeline;

    TiRHI::Buffer m_vertexBuffer;

    TiRHI::Buffer m_indexBuffer;

    TiRHI::Buffer m_rootConstantBuffer;

    TiRHI::DescriptorSet m_set;

    std::array<TiRHI::VertexBindingDescription, 1> m_vertexBindingDescription;

    std::array<TiRHI::VertexInputAttributeDescription, 2> m_vertexInputAttributeDescription;

    void onUpdate() final
    {
        static std::string windowName = [&]()
        { return std::string(getName()) + " " + std::string(TiRHI::toString(getRHI().getApi())); }();
        ImGui::Begin(windowName.c_str());

        ImGui::End();

        const float aspect = static_cast<float>(getWindow().getWidth()) / static_cast<float>(getWindow().getHeight());
        const MathLib::Matrix4x4f projection = Ti::Core::Math::perspectiveLH(1.39626f, aspect, 0.1f, 1000.0f);
        const MathLib::Matrix4x4f view = Ti::Core::Math::viewLH(MathLib::Vector3<float>{0.0f, 0.0f, -15.0f},
                                                                MathLib::Vector3<float>{0.0f, 0.0f, 0.0f},
                                                                MathLib::Vector3<float>{0.0f, 1.0f, 0.0f});
        const MathLib::Matrix4x4f model =
            MathLib::trs<float>(MathLib::Vector3<float>{0.0f, 0.0f, 0.0f},                    // Translation
                                MathLib::Vector3<float>{0.0f, (float)ImGui::GetTime(), 0.0f}, // Rotation (radians)
                                MathLib::Vector3<float>{1.0f, 1.0f, 1.0f}                     // Scale
            );
        const MathLib::Matrix4x4f wvp = projection * view * model;

        if (std::byte* ptr = m_rootConstantBuffer.beginDynamicUpdate())
        {
            MathLib::Matrix4x4f* m = reinterpret_cast<MathLib::Matrix4x4f*>(ptr);
            *m = wvp;

            m_rootConstantBuffer.endDynamicUpdate();
        }
    }

    void onRender(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
        // nothing
    }

    void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
        TiRHI::Viewport viewPort;
        viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

        cmd.setViewPort(viewPort);

        TiRHI::Rect2D scissors;
        scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
        cmd.setScissors(scissors);
        cmd.bindPipeline(m_pipeline);
        cmd.bindDescriptorSet(m_set, 0);
        cmd.bindVertexBuffer(0, 0, m_vertexBuffer);
        cmd.bindIndexBuffer(m_indexBuffer, 0, TiRHI::IndexType::UInt16);

        cmd.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
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
                    .build(getDevice());

            if (!result)
            {
                TI_LOG_FATAL(Core, "Failed to build texure");
            }
        }

        pushRenderCommand(
            [this, size = image.getTotalSize(), data = std::move(image).acquireData()](TiRHI::CommandList& command)
            {
                command.transitionResource(m_texture, TiRHI::ResourceState::CopyDestination);
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

    void setupSampler()
    {
        const bool success = m_sampler.setName("Texture Sampler")
                                 .setMinFilter(TiRHI::Filter::Linear)
                                 .setMagFilter(TiRHI::Filter::Linear)
                                 .setMipMapFilter(TiRHI::Filter::Linear)
                                 .setAddressModeU(TiRHI::SamplerAddressMode::Repeat)
                                 .setAddressModeV(TiRHI::SamplerAddressMode::Repeat)
                                 .setAddressModeW(TiRHI::SamplerAddressMode::Repeat)
                                 .setMinLod(0.0f)
                                 .setMaxLod(0.0f) // Only mip level 0 is uploaded.
                                 .setAnisotropyEnabled(false)
                                 .setDepthCompareEnabled(false)
                                 .build(getDevice());
        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build sampler");
        }
    }

    void setupPipeline()
    {
        const fs::path resourceDir = fs::path(TITANIUM_RESOURCE_DIR) / "cube.hlsl";
        const std::vector<Compiler::ShaderEntryPoint> entryPoint = {
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Vertex,
                                       .entryPoint = L"VSMain"sv,
                                       .outPutModuleName = L"cube.vert.spriv"},
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Pixel,
                                       .entryPoint = L"PSMain"sv,
                                       .outPutModuleName = L"cube.pixel.spriv"}};

        std::vector<CompileModuleData> moduleDatas = compileShader(resourceDir.generic_string().c_str(), entryPoint);

        for (auto& moduleDatas : moduleDatas)
            m_pipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        if (!m_pipeline.setName("Base Pipeline")
                 .setDescriptorSetLayoutEntries(
                     std::array{TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_set.getBindingsLayout(), 0}})
                 .setVertexBindingDescription(m_vertexBindingDescription)
                 .setVertexInputAttributeDescription(m_vertexInputAttributeDescription)
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

    void setupDescriptorSet()
    {
        const bool success = m_set.setName("Descriptor Set")
                                 .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &m_texture, &m_sampler, 0)
                                 .bindConstantBuffer(TiRHI::ShaderStageFlags::Vertex, &m_rootConstantBuffer, 1)
                                 .build(getDevice());

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
                           .build(getDevice());
        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build VertexBuffer");
            return;
        }

        pushRenderCommand(
            [this, verticiesToUpload = vertices](TiRHI::CommandList& command)
            {
                command.transitionResource(m_vertexBuffer, TiRHI::ResourceState::CopyDestination);
                command.uploadBuffer(m_vertexBuffer, 0,
                                     std::as_bytes(std::span(verticiesToUpload.data(), verticiesToUpload.size())));
                command.transitionResource(m_vertexBuffer, TiRHI::ResourceState::VertexBuffer);
            });

        success = m_indexBuffer.setName("IndexBuffer")
                      .setMemoryType(TiRHI::BufferMemoryType::Default)
                      .setUpdateMode(TiRHI::BufferUpdateMode::Static)
                      .setUsage(TiRHI::BufferUsage::Index | TiRHI::BufferUsage::CopyDestination)
                      .setSize(sizeof(indices))
                      .build(getDevice());

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
                      .build(getDevice());

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build ConstantBuffer Buffer");
            return;
        }
    }
};

int main()
{
    CubeRotated cubeRotated("CubeRotated");
    cubeRotated.run();
    return 0;
}
