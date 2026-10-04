#include <iostream>
#include <print>

#include <string>
#include <string_view>
#include <GLFW/glfw3.h>
#include <Window.hpp>
#include <Titanium/TitaniumHeader.hpp>

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

int main()
{
    Io io;

    TiSample::Io::Window window(WindowWidthBaseSize, WindowHeightBaseSize);

    TiRHI::RHI rhi(TiRHI::RhiCreate{.frameInFlight = 2, .logCallback = debugCallBack});

    // clang-format off
    TiRHI::Surface surface(rhi);
    surface
        .setName("WindowSurface")
        .build(window.getWindowHandle());
    // clang-format on

    TiRHI::Device device = rhi.newDevice();
    // clang-format off
    device.setName("My Device")
          .build(rhi, surface, rhi.getAdapters());
    // clang-format 

    
    std::println("Device Choosen : {}", device.getSourceAdapter(rhi).getName());

    // clang-format off
    TiRHI::SwapChain swapChain(rhi);
    swapChain
        .setWidth(static_cast<uint32_t>(window.getWidth()))
        .setHeight(static_cast<uint32_t>(window.getWidth()))
        .setName("SwapChain")
        .setVsync(true)
        .setImageCount(imageCount)
        .build(device, surface);
    // clang-format 


    TiRHI::CommandList cmdList(rhi);
    // clang-format off
    cmdList
        .setName("CommandList");
    cmdList.build(device);
    // clang-format on

    while (!window.shouldClose())
    {
        io.queryEvents();
        if (window.resized())
        {
            device.wait();
            swapChain.setWidth(window.getWidth()).setHeight(window.getHeight());
            swapChain.recreateSwapChain(device, surface);
        }

        const TiRHI::AcquiredFrame acquireFrame = swapChain.beginFrame();
        if (!acquireFrame.getSucces())
            continue;

        if (cmdList.beginRecord())
        {
#if defined(TITANIUM_VULKAN) // need to use render pass in order to avoid validation layer message
            vk::RenderPassBeginInfo renderPassBeginInfo{};
            vk::ClearColorValue clearColorValue;
            clearColorValue.setFloat32(clearColor);
            vk::ClearValue clearValue{};
            clearValue.setColor(clearColorValue);
            renderPassBeginInfo.setRenderPass(swapChain.getNativeRenderPass())
                .setFramebuffer(swapChain.getNativeFrameBuffer())
                .setRenderArea(
                    {{0, 0}, {static_cast<uint32_t>(window.getWidth()), static_cast<uint32_t>(window.getHeight())}})
                .setClearValues(clearValue);

            cmdList.getcurrentFrameCmb().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            cmdList.getcurrentFrameCmb().endRenderPass();
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

            cmdList.endRecord();
        }
        device.submit(acquireFrame, cmdList);
        swapChain.present();
        rhi.nextFrame();
    }

    device.wait();

    return 0;
}
