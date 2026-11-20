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

using namespace std::literals;
using namespace Ti::ShaderCompiler;
namespace fs = std::filesystem;

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
        , m_set(getRHI())
    {
        setupTexture();
        setupSampler();
        setupDescriptorSet();
        setupPipeline();
        setupImgui();
    }

protected:
    TiRHI::Texture m_texture;

    TiRHI::Sampler m_sampler;

    TiRHI::GraphicPipeline m_pipeline;

    TiRHI::DescriptorSet m_set;

    std::optional<std::move_only_function<void(TiRHI::CommandList&)>> pendingUpload;

    Ti::ThirdParty::ImguiIntegration m_imguiIntegration;

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

        pendingUpload.emplace(
            [this, size = image.getTotalSize(), data = std::move(image).acquireData()](TiRHI::CommandList& command)
            {
                command.transitionResource(m_texture, TiRHI::ResourceState::CopyDestination);
                command.upload2d(m_texture, std::as_bytes(std::span(data.get(), size)));
                command.transitionResource(m_texture, TiRHI::ResourceState::ShaderResource);
            });
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
            m_pipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        if (!m_pipeline.setName("Base Pipeline")
                 .setDescriptorSetLayoutEntries(
                     std::array{TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_set.getBindingsLayout(), 0}})
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

    void setupDescriptorSet()
    {
        const bool success = m_set.setName("Texture Descriptor Set")
                                 .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &m_texture, &m_sampler, 0)
                                 .build(getDevice());

        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build texture descriptor set");
        }
    }

    void setupImgui()
    {
        if (!m_imguiIntegration.build(getDevice(), getWindow(), getSwapChain()))
        {
            TI_LOG_ERROR(Core, "Failed to Init Imgui");
        }
    }

    void render(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
        if (!cmd.beginRecord())
            return;

        {
            if (pendingUpload)
            {
                TiRHI::DebugScope _(cmd, "Upload"sv, std::array{0.0f, 0.0f, 1.0f, 1.0f});
                (*pendingUpload)(cmd);
                pendingUpload.reset();
            }
        }

        {
            TiRHI::DebugScope _(cmd, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
            TiRHI::RenderTargets& renderTarget = getSwapChain().getCurrentRenderTargets();

            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(getSwapChain().getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;
            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor);

            cmd.transitionResource(getSwapChain().getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
            if (cmd.beginRenderPass(renderPass, renderTarget))
            {
                TiRHI::Viewport viewPort;
                viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

                cmd.setViewPort(viewPort);

                TiRHI::Rect2D scissors;
                scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
                cmd.setScissors(scissors);
                cmd.bindPipeline(m_pipeline);
                cmd.bindDescriptorSet(m_set, 0);
                cmd.draw(3, 1, 0, 0);

                {
                    TiRHI::DebugScope _(cmd, "Imgui Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
                    m_imguiIntegration.render(cmd);
                }

                cmd.endRenderPass();
            }
        }
        cmd.endRecord();
    }

    void update() final
    {
        m_imguiIntegration.newFrame();
        static std::string windowName = [&]()
        { return std::string(getName()) + " " + std::string(TiRHI::toString(getRHI().getApi())); }();
        ImGui::Begin(windowName.c_str());

        ImGui::End();
        m_imguiIntegration.update();
    }
};

int main()
{
    CubeRotated cubeRotated("CubeRotated");
    cubeRotated.run();
    return 0;
}
