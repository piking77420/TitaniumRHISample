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

using namespace Ti::ShaderCompiler;
using namespace std::literals;
namespace fs = std::filesystem;

class TriangleTexturedExemple : public Ti::Common::Exemple
{
public:
    TriangleTexturedExemple() = delete;
    virtual ~TriangleTexturedExemple() = default;

    TriangleTexturedExemple(std::string name)
        : Ti::Common::Exemple(std::move(name))
        , m_texture(getRHI())
        , m_samplerNearestRepeat(getRHI())
        , m_cubePipeline(getRHI())
        , m_cubeSet(getRHI())
    {
        setupTexture();
        setupSampler();
        setupDescriptorSet();
        setupPipeline();
    }

protected:
    TiRHI::Texture m_texture;

    TiRHI::Sampler m_samplerNearestRepeat;

    TiRHI::GraphicPipeline m_cubePipeline;

    TiRHI::DescriptorSet m_cubeSet;

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

        pushRenderCommand((
            [this, size = image.getTotalSize(), data = std::move(image).acquireData()](TiRHI::CommandList& command)
            {
                command.transitionResource(m_texture, TiRHI::ResourceState::CopyDestination);
                command.upload2d(m_texture, std::as_bytes(std::span(data.get(), size)));
                command.transitionResource(m_texture, TiRHI::ResourceState::ShaderResource);
            }));
    }

    void setupSampler()
    {
        const bool success = m_samplerNearestRepeat.setName("Texture Sampler")
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
        const fs::path resourceDir = fs::path(TITANIUM_RESOURCE_DIR) / "triangleTextured.hlsl";
        const std::vector<Compiler::ShaderEntryPoint> entryPoint = {
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Vertex,
                                       .entryPoint = L"VSMain"sv,
                                       .outPutModuleName = L"TriangleTextured.vert.spriv"},
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Pixel,
                                       .entryPoint = L"PSMain"sv,
                                       .outPutModuleName = L"TriangleTextured.pixel.spriv"}};

        std::vector<CompileModuleData> moduleDatas = compileShader(resourceDir.generic_string().c_str(), entryPoint);

        for (auto& moduleDatas : moduleDatas)
            m_cubePipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        if (!m_cubePipeline.setName("Base Pipeline")
                 .setDescriptorSetLayoutEntries(
                     std::array{TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_cubeSet.getBindingsLayout(), 0}})
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

    void setupDescriptorSet()
    {
        const bool success = m_cubeSet.setName("Texture Descriptor Set")
                                 .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &m_texture, &m_samplerNearestRepeat, 0)
                                 .build(getDevice());

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build texture descriptor set");
        }
    }

    void onUpdate() final
    {
    }

    void onRecord(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
    }

    void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        TiRHI::Viewport viewPort;
        viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

        cmd.setViewPort(viewPort);

        TiRHI::Rect2D scissors;
        scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
        cmd.setScissors(scissors);
        cmd.bindPipeline(m_cubePipeline);
        cmd.bindDescriptorSet(m_cubeSet, 0);
        cmd.draw(3, 1, 0, 0);
    }
};

int main()
{
    TriangleTexturedExemple triangleTexturedExemple("TriangleTexturedExemple");
    triangleTexturedExemple.run();
    return 0;
}
