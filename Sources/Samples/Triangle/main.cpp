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

    void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        TiRHI::Viewport viewPort;
        viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(swapChain.getExtend()));

        cmd.setViewPort(viewPort);

        TiRHI::Rect2D scissors;
        scissors.setOffset({0, 0}).setExtend(swapChain.getExtend());
        cmd.setScissors(scissors);
        cmd.bindPipeline(m_pipeline);
        cmd.draw(3, 1, 0, 0);
    }

    void onRender([[maybe_unused]] TiRHI::CommandList& cmd, [[maybe_unused]] TiRHI::SwapChain& swapChain)
    {
    }

    void onUpdate()
    {
    }
};

int main()
{
    TriangleExemple triangleExemple("TriangleExemple");
    triangleExemple.run();
    return 0;
}
