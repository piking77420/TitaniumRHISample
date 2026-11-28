#include <Rendering/RhiThread.hpp>
#include <Core/Log.hpp>

namespace Ti::Rendering
{
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

    RhiThread::RhiThread(TiRHI::WindowHandle windowHandle, uint32_t width, uint32_t height)
        : m_rhi(
              []() -> TiRHI::RhiCreate
              {
                  TiRHI::RhiCreate create;
                  create.useDebugLabel = true;
                  create.frameInFlight = 3;
                  create.logCallback = debugCallBack;

                  return create;
              }())
        , m_device(getRHI())
        , m_surface(getDevice())
        , m_swapChain(getDevice())
        , m_cmdList(getDevice())

    {

        // clang-format off
        m_surface.setName("WindowSurface").build(windowHandle);
        // clang-format on

        // clang-format off
        m_device.setName("My Device")
                .build(m_surface, m_rhi.getAdapters());
        TI_LOG_INFO(Core, "Device Choosen : {}",  m_device.getSourceAdapter(m_rhi).getName());
        // clang-format


         // clang-format off
         m_swapChain
            .setWidth(width)
            .setHeight(height)
            .setName("SwapChain")
            .setVsync(true)
            .setImageCount(ImageCount)
            .build(m_surface);
         // clang-format

            // clang-format off
            m_cmdList
                .setName("CommandList");
            m_cmdList.build();
        // clang-format on
    }

    RhiThread::~RhiThread() = default;

    void RhiThread::consumeCommands()
    {
        for (auto& f : m_rhiCommands)
            f(*this);
        m_rhiCommands.clear();
    }

    void RhiThread::consumeOnRecordRhiCommands()
    {
        for (auto& f : m_rhiCommandOnRecord)
            f(getDevice(), m_cmdList);
        m_rhiCommandOnRecord.clear();
    }

} // namespace Ti::Rendering
