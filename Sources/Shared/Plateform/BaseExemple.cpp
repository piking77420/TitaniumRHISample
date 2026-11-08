#include <Plateform/BaseExemple.hpp>
#include <Core/Log.hpp>

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

namespace Ti::Plateform
{
    BaseExemple::~BaseExemple()
    {
    }

    BaseExemple::BaseExemple(std::string name)
        : App(std::move(name))
        , m_window(static_cast<int>(WindowWidthBaseSize), static_cast<int>(WindowHeightBaseSize))
        , m_rhi(
              []() -> TiRHI::RhiCreate
              {
                  TiRHI::RhiCreate create;
                  create.useDebugLabel = true;
                  create.frameInFlight = 3;
                  create.logCallback = debugCallBack;

                  return create;
              }())
        , m_device(m_rhi)
        , m_surface(m_rhi)
        , m_swapChain(m_rhi)
        , m_cmdList(m_rhi)
    {
        // clang-format off
        
        m_surface
            .setName("WindowSurface")
            .build(m_window.getWindowHandle());
        // clang-format on

        // clang-format off
        m_device.setName("My Device")
                .build(m_rhi, m_surface, m_rhi.getAdapters());
        TI_LOG_INFO(Core, "Device Choosen : {}",  m_device.getSourceAdapter(m_rhi).getName());
        // clang-format 
        

         // clang-format off
         m_swapChain
            .setWidth(static_cast<uint32_t>(m_window.getWidth()))
            .setHeight(static_cast<uint32_t>(m_window.getHeight()))
            .setName("SwapChain")
            .setVsync(true)
            .setImageCount(ImageCount)
            .build(m_device, m_surface);
         // clang-format 

            // clang-format off
            m_cmdList
                .setName("CommandList");
            m_cmdList.build(m_device);
        // clang-format on
    }

    void BaseExemple::handleResize()
    {
        if (!m_window.resized())
        {
            return;
        }
        // To avoid device wait we need to use deferred destroy
        m_device.wait();
        m_swapChain.setWidth(m_window.getWidth()).setHeight(m_window.getHeight());
        m_swapChain.recreateSwapChain(m_device, m_surface);
    }

    void BaseExemple::run()
    {
        while (!m_window.shouldClose())
        {
            m_window.prepareForCurrentFrame();
            m_io.poolEvents();

            update();
            {
                handleResize();
                m_device.beginFrame();

                std::array frames = {m_swapChain.acquireNextImage(m_device)};

                if (frames[0].getSucces())
                {
                    render(m_cmdList, m_swapChain);

                    const std::array submitDesc = {
                        TiRHI::Device::SubmitPresentDesc{.commandList = &m_cmdList, .acquireFrames = frames}};

                    m_device.submitPresent(submitDesc);
                    m_rhi.nextFrame();
                }
            }
        }
        m_device.wait();
    }

} // Ti::Core::Platerform
