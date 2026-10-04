#include <iostream>
#include <print>

#include <string>
#include <string_view>
#include <GLFW/glfw3.h>
#include <Window.hpp>
#include <Titanium/TitaniumHeader.hpp>

constexpr int MaxAppInstanceCount = 100;
constexpr size_t imageCount = 3;
constexpr size_t WindowWidthBaseSize = 1280;
constexpr size_t WindowHeightBaseSize = 720;
constexpr std::array clearColor{0.1f, 0.2f, 0.4f, 1.0f};
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

    std::wcout << std::format(L"[RHI][{}]{}[{}]{}[{}]\n", TiRHI::toWstring(api), getColor(), TiRHI::toWstring(severity),
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

    std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*> render()
    {
        const TiRHI::AcquiredFrame acquireFrame = m_swapChain.acquireNextImage();
        if (!acquireFrame.getSucces())
            return std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*>(acquireFrame, nullptr);
        ;

        if (m_cmdList.beginRecord())
        {
#if defined(TITANIUM_VULKAN) // need to use render pass in order to avoid validation layer message
            vk::RenderPassBeginInfo renderPassBeginInfo{};
            vk::ClearColorValue clearColorValue;
            clearColorValue.setFloat32(clearColor);
            vk::ClearValue clearValue{};
            clearValue.setColor(clearColorValue);
            renderPassBeginInfo.setRenderPass(m_swapChain.getNativeRenderPass())
                .setFramebuffer(m_swapChain.getNativeFrameBuffer())
                .setRenderArea(
                    {{0, 0}, {static_cast<uint32_t>(m_window.getWidth()), static_cast<uint32_t>(m_window.getHeight())}})
                .setClearValues(clearValue);

            m_cmdList.getcurrentFrameCmb().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            m_cmdList.getcurrentFrameCmb().endRenderPass();
#endif

#if defined(TITANIUM_DIRECT_X12)
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = swapChain.getNativeCurrentBackBuffer();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            auto nativeCml = cmdList.getCommandListNative();
            nativeCml->ResourceBarrier(1, &barrier);

            auto rtv = swapChain.getRtv();
            nativeCml->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

            nativeCml->ClearRenderTargetView(rtv, clearColor.data(), 0, nullptr);
            std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);

            nativeCml->ResourceBarrier(1, &barrier);
#endif

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

int main()
{
    std::println("Number of Instance ? ");

    int openApps;
    Io io;
    std::cin >> openApps;
    openApps = std::clamp(openApps, 0, MaxAppInstanceCount);

    TiRHI::RHI rhi(TiRHI::RhiCreate{.frameInFlight = 2, .logCallback = debugCallBack});

    TiRHI::Device device = rhi.newDevice();

    std::println("Device Choosen : {}", device.getSourceAdapter(rhi).getName());

    std::vector<std::unique_ptr<App>> apps;

    for (size_t i = 0; i < static_cast<size_t>(openApps); i++)
    {
        apps.emplace_back(std::make_unique<App>(rhi, device));
    }
    std::vector<TiRHI::AcquiredFrame> acquiredFrames;
    std::vector<TiRHI::CommandList*> commandLists;

    while (!apps.empty())
    {
        acquiredFrames.clear();
        commandLists.clear();
        for (auto& app : apps)
            app->prepareForCurrentFrame();

        io.queryEvents();
        for (auto it = apps.begin(); it != apps.end();)
        {
            if ((*it)->shouldClose())
            {
                device.wait();
                it = apps.erase(it);
                continue;
            }

            (*it)->handleResize(device);
            ++it;
        }

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
