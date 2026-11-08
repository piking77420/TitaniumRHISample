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

#include <Assets/Image.hpp>
#include <Assets/Texture.hpp>
#include <Rendering/Texture.hpp>
#include <ShaderCompiler.hpp>
#include <Core/Thread.hpp>
#include <Plateform/BaseExemple.hpp>
#include <Io/File.hpp>
#include <Core/Log.hpp>

using namespace std::literals;

class TriangleExemple : public Ti::Plateform::BaseExemple
{
public:
    TriangleExemple() = delete;
    virtual ~TriangleExemple() = default;

    TriangleExemple(std::string name)
        : Ti::Plateform::BaseExemple(std::move(name))
        , m_pipeline(makeGraphicPipeline(getRHI()))
    {
        if (!m_pipeline.setName("Base Pipeline")
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

protected:
    Ti::ShaderCompiler::Compiler m_shaderCompiler;

    TiRHI::GraphicPipeline m_pipeline;

    TiRHI::GraphicPipeline makeGraphicPipeline(TiRHI::RHI& rhi)
    {
        namespace fs = std::filesystem;

        const fs::path resourceDir = TITANIUM_RESOURCE_DIR;

        Ti::Io::File file;
        file.loadFileBinary(resourceDir / "triangle.hlsl");
        using namespace Ti::ShaderCompiler;
        auto getShaderCompilerApi = [&]()
        {
            switch (rhi.getApi())
            {
            case TiRHI::RhiApi::DirectX12:
                return Compiler::ApiTarget::Dxil;
            case TiRHI::RhiApi::Metal:
                return Compiler::ApiTarget::Spirv;
            case TiRHI::RhiApi::Vulkan:
                return Compiler::ApiTarget::Spirv;
            default:
                break;
            }
            return Compiler::ApiTarget::Spirv;
        };

        Compiler c{};

        const std::vector<Compiler::ShaderEntryPoint> entryPoint = {
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Vertex,
                                       .entryPoint = L"VSMain"sv,
                                       .outPutModuleName = L"Triangle.vert.spriv"},
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Pixel,
                                       .entryPoint = L"PSMain"sv,
                                       .outPutModuleName = L"Triangle.pixel.spriv"}};

        TiRHI::GraphicPipeline pipeline(rhi);

        Compiler::GetCodeInfo info;
        // clang-format off
    info.setCode(std::span((const std::byte*)file.getData(), (const std::byte*)(file.getData() + file.getSize())))
        .setApiTarget(getShaderCompilerApi())
        .setEntryPoints(entryPoint);

    std::expected modulesCode = c.getCode(info);
    
    if (modulesCode)
    {
        std::vector<std::vector<std::byte>>& codes = modulesCode.value();

        auto toStringFromW = [](const std::wstring_view& ws)
            {
                return std::string{ws.begin(), ws.end()};
            };

            pipeline.addModule(TiRHI::ShaderStage::Vertex, codes[0], toStringFromW(entryPoint[0].entryPoint))
                .addModule(TiRHI::ShaderStage::Pixel, codes[1], toStringFromW(entryPoint[1].entryPoint));
    }
    else
	{
	    const Compiler::CompileError& error = modulesCode.error();

        if (std::holds_alternative<Compiler::GetCodeError>(error))
        {
            const Compiler::GetCodeError codeError = std::get<Compiler::GetCodeError>(error);
            switch (codeError)
            {
            case Compiler::GetCodeError::EmptyEntryPoints:
                std::cerr << "Empty entry points\n";
                break;

            case Compiler::GetCodeError::InvalidSourceCode:
                std::cerr << "Invalid source code\n";
                break;

            case Compiler::GetCodeError::LoadingCodeFailed:
                std::cerr << "Failed to load shader code\n";
                break;

            case Compiler::GetCodeError::InvalidShaderStage:
                std::cerr << "Invalid shader stage\n";
                break;

            case Compiler::GetCodeError::InvalidApi:
                std::cerr << "Invalid API target\n";
                break;

            case Compiler::GetCodeError::ShaderCompilationFailed:
                std::cerr << "Shader compilation failed\n";
                break;

            case Compiler::GetCodeError::UnexpectedError:
                std::cerr << "Unexpected error\n";
                break;

            default:
                std::cerr << "Unknown shader compiler error\n";
                break;
            }
        }
        else
        {
            std::cerr << std::get<std::string>(error) << '\n';
        }
	}


    return pipeline;
}

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
