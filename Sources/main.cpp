#include <iostream>

#include <string>
#include <string_view>
#include <GLFW/glfw3.h>
#include <Window.hpp>

constexpr size_t WindowWidthBaseSize = 1280;
constexpr size_t WindowHeightBaseSize = 720;
constexpr float clearColor[4]{0.1f, 0.2f, 0.4f, 1.0f};
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

int main()
{
    TiRHI::RHI rhi(TiRHI::RhiCreate{.frameInFlight = 2, .logCallback = debugCallBack});
    TiRHI::Device device = rhi.newDevice();
    // clang-format off
    device.setName("BaseDevice")
          .build(rhi, rhi.getAdapters());
    // clang-format 
    TiSample::Io::Window window(WindowWidthBaseSize, WindowHeightBaseSize, rhi, device);

    TiRHI::CommandList cmdList(rhi);
    // clang-format off
    cmdList
        .setName("CommandList");
    cmdList.build(device);
    // clang-format on

    while (!window.shouldClose())
    {
        window.poolEvent();
        if (!window.beginFrame())
            continue;

        if (cmdList.beginRecord())
        {
            cmdList.endRecord();
        }
        device.submit(cmdList);
        window.endFrame(device);
    }

    device.wait();

    return 0;
}
