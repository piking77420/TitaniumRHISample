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

#include <Common/Exemple.hpp>
#include <Assets/Image.hpp>
#include <Assets/Texture.hpp>
#include <Rendering/Texture.hpp>
#include <Io/File.hpp>
#include <Core/Log.hpp>

using namespace std::literals;

class TriangleExemple : public Ti::Common::Exemple
{
public:
    TriangleExemple() = delete;
    virtual ~TriangleExemple() = default;

    TriangleExemple(std::string name)
        : Ti::Common::Exemple(std::move(name))
        , m_pipeline(getRHI())
    {
        using namespace Ti::ShaderCompiler;
        namespace fs = std::filesystem;

        const fs::path resourceDir = fs::path(TITANIUM_RESOURCE_DIR) / "triangle.hlsl";
        const std::vector<Compiler::ShaderEntryPoint> entryPoint = {
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Vertex,
                                       .entryPoint = L"VSMain"sv,
                                       .outPutModuleName = L"Triangle.vert.spriv"},
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Pixel,
                                       .entryPoint = L"PSMain"sv,
                                       .outPutModuleName = L"Triangle.pixel.spriv"}};

        std::vector<CompileModuleData> moduleDatas = compileShader(resourceDir.generic_string().c_str(), entryPoint);

        for (auto& moduleDatas : moduleDatas)
            m_pipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        if (!m_pipeline.setName("Base Pipeline")
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

protected:
    TiRHI::GraphicPipeline m_pipeline;

    void render(TiRHI::CommandList& cmd, TiRHI::SwapChain& m_swapChain) final
    {
        if (!cmd.beginRecord())
            return;
        {
            TiRHI::DebugScope _(cmd, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
            TiRHI::RenderTargets& renderTarget = m_swapChain.getCurrentRenderTargets();

            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(m_swapChain.getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;
            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor);

            cmd.transitionResource(m_swapChain.getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
            if (cmd.beginRenderPass(renderPass, renderTarget))
            {
                TiRHI::Viewport viewPort;
                viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(m_swapChain.getExtend()));

                cmd.setViewPort(viewPort);

                TiRHI::Rect2D scissors;
                scissors.setOffset({0, 0}).setExtend(m_swapChain.getExtend());
                cmd.setScissors(scissors);
                cmd.bindPipeline(m_pipeline);
                cmd.draw(3, 1, 0, 0);
                cmd.endRenderPass();
            }
        }
        cmd.endRecord();
    }

    void update() final
    {
        // NOTHING
    }
};

int main()
{
    TriangleExemple triangleExemple("TriangleExemple");
    triangleExemple.run();
    return 0;
}
