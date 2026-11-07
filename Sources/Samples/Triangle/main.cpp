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
#include <PlateForm/App.hpp>
#include <PlateForm/Window.hpp>
#include <Io/Io.hpp>
#include <Io/File.hpp>
#include <Core/Log.hpp>

constexpr int MaxAppInstanceCount = 8;
using namespace std::literals;

void debugCallBack(const std::wstring& message, TiRHI::RhiApi api, TiRHI::RhiMessageSeverity severity)
{
    const std::string s = std::string{message.begin(), message.end()};
    const std::string_view apiName = TiRHI::toString(api);

    switch (severity)
    {
    case TiRHI::RhiMessageSeverity::Verbose:
        TI_LOG_VERBOSE(RHI, "[{}] {}", apiName, s);
        break;

    case TiRHI::RhiMessageSeverity::Info:
        TI_LOG_INFO(RHI, "[{}] {}", apiName, s);
        break;

    case TiRHI::RhiMessageSeverity::Warning:
        TI_LOG_WARNING(RHI, "[{}] {}", apiName, s);
        break;

    case TiRHI::RhiMessageSeverity::Error:
        TI_LOG_ERROR(RHI, "[{}] {}", apiName, s);
        break;

    case TiRHI::RhiMessageSeverity::Fatal:
        TI_LOG_FATAL(RHI, "[{}] {}", apiName, s);
        break;
    }
}

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

int main()
{
    Ti::Core::SetThreadName("MainThread");
    Ti::Core::Log::setFileOutPut("Saved/Log.txt");
    Ti::Io::Io io;
    TiRHI::RHI rhi(TiRHI::RhiCreate{.useDebugLabel = true, .frameInFlight = 2, .logCallback = debugCallBack});
    TiRHI::Device device = rhi.newDevice();

    TI_LOG_INFO(Core, "Device Choosen : {}",  device.getSourceAdapter(rhi).getName());
    std::println("Device Choosen : {}", device.getSourceAdapter(rhi).getName());

    std::println("Number of Instance ? ");
    int openApps;
    std::cin >> openApps;
    openApps = std::clamp(openApps, 1, MaxAppInstanceCount);

    std::vector<std::unique_ptr<Ti::Plateform::App>> apps;

    for (size_t i = 0; i < static_cast<size_t>(openApps); i++)
    {
        apps.emplace_back(std::make_unique<Ti::Plateform::App>(rhi, device));
    }
    std::vector<std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*>> renderApps;
    std::vector<TiRHI::Device::SubmitPresentDesc> submitsPresents;
    Ti::Assets::AssetStrongRef<Ti::Assets::Image> image = std::make_shared<Ti::Assets::Image>(std::filesystem::path(TITANIUM_RESOURCE_DIR) / "DiamondOre.png");
    if (image->getSize() == 0ull)
    {
        TI_LOG_FATAL(Core, "Failed to load image");
        return 1;
    }
    Ti::Assets::Texture texture(image);
    Ti::Rendering::Texture renderTexture = Ti::Rendering::Texture(device, texture);

    if (!renderTexture.isValid())
    {
        TI_LOG_FATAL(Core, "Failed build texture image");
        return 1;
    }

    TiRHI::Sampler s(rhi);
    if (!s.setName("BaseSampler")
        .build(device))
    {
        TI_LOG_FATAL(Core, "Failed build Sampler");
        return 1;
    }

    TiRHI::DescriptorSet set(rhi);

    if (!set.setName("TextureDescritproSet")
        .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &renderTexture.getRhiTexture(), &s, 0, 1)
        .build(device))
    {
        TI_LOG_FATAL(Core, "Failed build DescriptorSet");
        return 1;
    }


    TiRHI::GraphicPipeline graphicPipiline = makeGraphicPipeline(rhi);
    graphicPipiline.setRenderPassDescritptor(&apps[0]->getSwapChain().getRenderPassDescriptor());
    if (!graphicPipiline.build(device))
    {
        TI_LOG_FATAL(Core, "Failed to build graphic Pipeline");
        return 1;
    }

    for (auto& app : apps)
    {
        app->graphicPipeline = &graphicPipiline;
    }

    while (!apps.empty())
    {
        renderApps.clear();
        submitsPresents.clear();

        for (auto& app : apps)
            app->prepareForCurrentFrame();

        io.poolEvents();

        bool needWait = false;
        for (const auto& app : apps)
        {
            if (app->shouldClose())
            {
                needWait = true;
                break;
            }
        }

        if (needWait)
        {
            device.wait();
            std::erase_if(apps, [](const auto& app) { return app->shouldClose(); });
        }

        for (auto& app : apps)
            app->handleResize(device);
       
        device.beginFrame();

        for (auto& app : apps)
        {
            auto tickOut = app->render(device);
            if (tickOut.first.getSucces() && tickOut.second != nullptr)
                 renderApps.emplace_back(tickOut.first, tickOut.second);
            
        }

        for (auto& renderApp : renderApps)
		{
			 submitsPresents.emplace_back(TiRHI::Device::SubmitPresentDesc
                 {
                     .commandList = renderApp.second,
                     .acquireFrames = std::span(&renderApp.first, 1)
                 });
		}

        device.submitPresent(submitsPresents);
        rhi.nextFrame();
    }

    device.wait();

    return 0;
}
