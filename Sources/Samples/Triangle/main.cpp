#include <iostream>
#include <print>
#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <format>
#include <string>
#include <string_view>

#include <ShaderCompiler.hpp>
#include <Core/Thread.hpp>
#include <PlateForm/App.hpp>
#include <PlateForm/Window.hpp>
#include <Io/Io.hpp>
#include <Io/File.hpp>

constexpr int MaxAppInstanceCount = 8;
using namespace std::literals;

static constexpr std::wstring_view AnsiReset = L"\x1b[0m"sv;
static constexpr std::wstring_view AnsiBlack = L"\x1b[30m"sv;
static constexpr std::wstring_view AnsiRed = L"\x1b[31m"sv;
static constexpr std::wstring_view AnsiGreen = L"\x1b[32m"sv;
static constexpr std::wstring_view AnsiYellow = L"\x1b[33m"sv;
static constexpr std::wstring_view AnsiOrange = L"\x1b[38;2;255;165;0m"sv;
static constexpr std::wstring_view AnsiBlue = L"\x1b[34m"sv;
static constexpr std::wstring_view AnsiMagenta = L"\x1b[35m"sv;
static constexpr std::wstring_view AnsiCyan = L"\x1b[36m"sv;
static constexpr std::wstring_view AnsiWhite = L"\x1b[37m"sv;
static constexpr std::wstring_view AnsiGrey = L"\x1b[90m"sv;
static constexpr std::wstring_view AnsiBrightRed = L"\x1b[91m"sv;
static constexpr std::wstring_view AnsiBrightGreen = L"\x1b[92m"sv;
static constexpr std::wstring_view AnsiBrightYellow = L"\x1b[93m"sv;
static constexpr std::wstring_view AnsiBrightBlue = L"\x1b[94m"sv;
static constexpr std::wstring_view AnsiBrightMagenta = L"\x1b[95m"sv;
static constexpr std::wstring_view AnsiBrightCyan = L"\x1b[96m"sv;
static constexpr std::wstring_view AnsiBrightWhite = L"\x1b[97m"sv;

void debugCallBack(const std::wstring& message, TiRHI::RhiApi api, TiRHI::RhiMessageSeverity severity)
{
    auto getColor = [&severity]() -> std::wstring_view
    {
        switch (severity)
        {
        case TiRHI::RhiMessageSeverity::Verbose:
            return AnsiReset;
        case TiRHI::RhiMessageSeverity::Info:
            return AnsiGrey;
        case TiRHI::RhiMessageSeverity::Warning:
            return AnsiOrange;
        case TiRHI::RhiMessageSeverity::Error:
            return AnsiRed;
        case TiRHI::RhiMessageSeverity::Fatal:
            return AnsiMagenta;
        }

        return AnsiReset;
    };

    std::wcout << std::format(L"[RHI][{}]{}[{}]{}[{}]\n", TiRHI::toWString(api), getColor(), TiRHI::toWstring(severity),
                              AnsiReset, message);
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

    Ti::Io::Io io;
    TiRHI::RHI rhi(TiRHI::RhiCreate{.useDebugLabel = true, .frameInFlight = 2, .logCallback = debugCallBack});
    TiRHI::Device device = rhi.newDevice();
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
    std::vector<TiRHI::AcquiredFrame> acquiredFrames;
    std::vector<TiRHI::CommandList*> commandLists;

    TiRHI::GraphicPipeline graphicPipiline = makeGraphicPipeline(rhi);
    graphicPipiline.setRenderPassDescritptor(&apps[0]->getSwapChain().getRenderPassDescriptor());
    if (!graphicPipiline.build(device))
    {
        std::println("Failed to build graphic Pipeline");
        std::getchar();
        return 1;
    }

    for (auto& app : apps)
    {
        app->graphicPipeline = &graphicPipiline;
    }

    while (!apps.empty())
    {
        acquiredFrames.clear();
        commandLists.clear();
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
            {
                acquiredFrames.emplace_back(tickOut.first);
                commandLists.emplace_back(tickOut.second);
            }
        }

        device.submit(acquiredFrames, commandLists);
        for (auto& app : apps)
            app->swapBuffer();
        rhi.nextFrame();
    }

    device.wait();

    return 0;
}
