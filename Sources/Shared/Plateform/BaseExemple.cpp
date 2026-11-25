#include <Plateform/BaseExemple.hpp>
#include <Core/Log.hpp>
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

namespace Ti::Plateform
{
    BaseExemple::~BaseExemple()
    {
    }

    BaseExemple::BaseExemple(std::string name)
        : App(std::move(name))
        , m_window(static_cast<int>(WindowWidthBaseSize), static_cast<int>(WindowHeightBaseSize), getName().data())
        , m_rhi(
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
        
        m_surface
            .setName("WindowSurface")
            .build(m_window.getWindowHandle());
        // clang-format on

        // clang-format off
        m_device.setName("My Device")
                .build(m_surface, m_rhi.getAdapters());
        TI_LOG_INFO(Core, "Device Choosen : {}",  m_device.getSourceAdapter(m_rhi).getName());
        // clang-format 
        

         // clang-format off
         m_swapChain
            .setWidth(static_cast<uint32_t>(m_window.getWidth()))
            .setHeight(static_cast<uint32_t>(m_window.getHeight()))
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

        if (!m_imguiIntegration.build(getDevice(), getWindow(), getSwapChain()))
        {
            TI_LOG_ERROR(Core, "Failed to Init Imgui");
        }
    }

    void BaseExemple::handleResize()
    {
        if (!m_window.resized())
        {
            return;
        }
        // To avoid device wait we need to use deferred destroy
        // m_device.wait();
        m_swapChain.setWidth(m_window.getWidth()).setHeight(m_window.getHeight());
        m_swapChain.recreateSwapChain(m_surface);
        onResizeWindow();
    }

    void BaseExemple::record(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        if (!cmd.beginRecord())
            return;

        if (!m_pendingRenderCommand.empty())
        {
            TiRHI::DebugScope _(cmd, "Upload"sv, std::array{0.0f, 0.0f, 1.0f, 1.0f});
            for (auto& f : m_pendingRenderCommand)
                f(cmd);
            m_pendingRenderCommand.clear();
        }

        onRecord(cmd, swapChain);

        {
            TiRHI::DebugScope _(cmd, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
            TiRHI::RenderTargets& renderTarget = getSwapChain().getCurrentRenderTargets();

            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(getSwapChain().getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;
            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor);

            cmd.transitionResource(getSwapChain().getCurrentSwapChainTexture(), TiRHI::ResourceState::RenderTarget);
            if (cmd.beginRenderPass(renderPass, renderTarget))
            {
                onSwapChainRenderPass(cmd, swapChain);

                {
                    TiRHI::DebugScope _(cmd, "Imgui Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
                    m_imguiIntegration.record(cmd);
                }

                cmd.endRenderPass();
            }
            cmd.transitionResource(getSwapChain().getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
        }
        cmd.endRecord();
    }

    void BaseExemple::update()
    {
        onUpdate();
        m_imguiIntegration.update();
    }

    void BaseExemple::render()
    {
        onRender();
    }

    void BaseExemple::run()
    {
        while (!m_window.shouldClose())
        {
            m_window.prepareForCurrentFrame();
            m_io.poolEvents();
            m_imguiIntegration.newFrame();
            handleResize();
            update();
            {
                m_device.beginFrame();
                render();

                std::array frames = {m_swapChain.acquireNextImage()};

                if (frames[0].getSucces())
                {
                    record(m_cmdList, m_swapChain);

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
