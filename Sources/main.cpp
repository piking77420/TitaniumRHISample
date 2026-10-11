#include <iostream>
#include <print>

#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>

#include <format>
#include <string>
#include <string_view>
#include <GLFW/glfw3.h>
#include <Window.hpp>
#include <Titanium/TitaniumHeader.hpp>
#include <Titanium/RHI-DebugScope.hpp>

constexpr int MaxAppInstanceCount = 8;
constexpr size_t imageCount = 3;
constexpr size_t WindowWidthBaseSize = 1280;
constexpr size_t WindowHeightBaseSize = 720;
constexpr std::array AppClearColor{0.1f, 0.2f, 0.4f, 1.0f};
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

class Io
{
public:
    Io()
    {
        glfwInit();
    }

    void queryEvents()
    {
        glfwPollEvents();
    }

    ~Io()
    {
        glfwTerminate();
    }

private:
};

class App
{
public:
    App(TiRHI::RHI& rhi, TiRHI::Device& device)
        : m_window(WindowWidthBaseSize, WindowHeightBaseSize)
        , m_surface(rhi)
        , m_swapChain(rhi)
        , m_cmdList(rhi)
    {
        // clang-format off
        
        m_surface
            .setName("WindowSurface")
            .build(m_window.getWindowHandle());
        // clang-format on

        if (!device.isValid())
        {
            // clang-format off
            device.setName("My Device")
                  .build(rhi, m_surface, rhi.getAdapters());
            // clang-format 
        }

         // clang-format off
         m_swapChain
            .setWidth(static_cast<uint32_t>(m_window.getWidth()))
            .setHeight(static_cast<uint32_t>(m_window.getHeight()))
            .setName("SwapChain")
            .setVsync(true)
            .setImageCount(imageCount)
            .build(device, m_surface);
         // clang-format 

            // clang-format off
            m_cmdList
                .setName("CommandList");
            m_cmdList.build(device);
        // clang-format on
    }
    ~App() = default;

    TiRHI::GraphicPipeline* graphicPipeline = nullptr;

    std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*> render()
    {
        const TiRHI::AcquiredFrame acquireFrame = m_swapChain.acquireNextImage();
        if (!acquireFrame)
            return std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*>(acquireFrame, nullptr);

        if (m_cmdList.beginRecord())
        {
            {
                TiRHI::DebugScope _(m_cmdList, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
                TiRHI::RenderTargets& renderTarget = m_swapChain.getCurrentRenderTargets();

                TiRHI::Rect2D renderArea{};
                renderArea.setExtend(m_swapChain.getExtend());

                std::array<TiRHI::ClearValueColor, 1> clearColor;
                clearColor[0].color = AppClearColor;
                TiRHI::BeginRenderPass renderPass{};
                renderPass.setRenderArea(renderArea).setClearColors(clearColor);

                m_cmdList.transitionResource(m_swapChain.getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
                if (m_cmdList.beginRenderPass(renderPass, renderTarget))
                {
                    TiRHI::Viewport viewPort;
                    viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(m_swapChain.getExtend()));

                    m_cmdList.setViewPort(viewPort);

                    TiRHI::Rect2D scissors;
                    scissors.setOffset({0, 0}).setExtend(m_swapChain.getExtend());
                    m_cmdList.setScissors(scissors);

                    if (graphicPipeline)
                    {
                        m_cmdList.bindPipeline(*graphicPipeline);
                        m_cmdList.draw(3, 1, 0, 0);
                    }

                    m_cmdList.endRenderPass();
                }
            }

            m_cmdList.endRecord();
        }

        return std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*>(acquireFrame, &m_cmdList);
    }

    void swapBuffer()
    {
        m_swapChain.present();
    }

    void handleResize(TiRHI::Device& device)
    {
        if (!m_window.resized())
        {
            return;
        }

        device.wait();
        m_swapChain.setWidth(m_window.getWidth()).setHeight(m_window.getHeight());
        m_swapChain.recreateSwapChain(device, m_surface);
    }

    void prepareForCurrentFrame()
    {
        m_window.prepareForCurrentFrame();
    }

    bool shouldClose() const
    {
        return m_window.shouldClose();
    }

private:
    TiSample::Io::Window m_window;

    TiRHI::Surface m_surface;

    TiRHI::SwapChain m_swapChain;

    TiRHI::CommandList m_cmdList;
};

std::pair<std::unique_ptr<uint8_t[]>, uint32_t> loadBinaryFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file)
        throw std::runtime_error("Failed to open shader file");

    const auto size = file.tellg();

    if (size <= 0 || size > std::numeric_limits<uint32_t>::max())
        throw std::runtime_error("Invalid shader file size");

    auto data = std::make_unique<uint8_t[]>(static_cast<size_t>(size));

    file.seekg(0, std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(data.get()), size))
        throw std::runtime_error("Failed to read shader file");

    return {std::move(data), static_cast<uint32_t>(size)};
}

TiRHI::GraphicPipeline makeGraphicPipeline(TiRHI::RHI& rhi)
{
    namespace fs = std::filesystem;

    const fs::path resourceDir = TITANIUM_RESOURCE_DIR;

    TiRHI::GraphicPipeline pipeline(rhi);

    std::cout << std::boolalpha << std::filesystem::exists(resourceDir) << '\n';

    auto shaderPath = resourceDir / "Vertex.dxil";

    std::cout << std::filesystem::exists(shaderPath) << '\n';

    auto [vertexData, vertexSize] = loadBinaryFile(resourceDir / "TriangleVS.dxil");

    auto [pixelData, pixelSize] = loadBinaryFile(resourceDir / "TrianglePS.dxil");

    pipeline.addModule(TiRHI::ShaderStage::Vertex, std::move(vertexData), vertexSize)
        .addModule(TiRHI::ShaderStage::Pixel, std::move(pixelData), pixelSize);

    return pipeline;
}
int main()
{
    std::println("Number of Instance ? ");

    int openApps;
    Io io;
    std::cin >> openApps;
    openApps = std::clamp(openApps, 1, MaxAppInstanceCount);

    TiRHI::RHI rhi(TiRHI::RhiCreate{.useDebugLabel = true, .frameInFlight = 2, .logCallback = debugCallBack});

    TiRHI::Device device = rhi.newDevice();

    std::println("Device Choosen : {}", device.getSourceAdapter(rhi).getName());

    std::vector<std::unique_ptr<App>> apps;

    for (size_t i = 0; i < static_cast<size_t>(openApps); i++)
    {
        apps.emplace_back(std::make_unique<App>(rhi, device));
    }
    std::vector<TiRHI::AcquiredFrame> acquiredFrames;
    std::vector<TiRHI::CommandList*> commandLists;

    TiRHI::GraphicPipeline graphicPipiline = makeGraphicPipeline(rhi);
    if (!graphicPipiline.build(device))
    {
        std::println("Failed to build graphic Pipeline");
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

        io.queryEvents();
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
            auto tickOut = app->render();
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
