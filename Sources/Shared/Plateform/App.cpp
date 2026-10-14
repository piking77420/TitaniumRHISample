#include <App.hpp>

namespace Ti::Plateform
{
    using namespace std::literals;

    App::App(TiRHI::RHI& rhi, TiRHI::Device& device)
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

    App::~App() = default;

    std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*> App::render(TiRHI::Device& device)
    {
        const TiRHI::AcquiredFrame acquireFrame = m_swapChain.acquireNextImage(device);
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

    void App::swapBuffer()
    {
        m_swapChain.present();
    }

    void App::handleResize(TiRHI::Device& device)
    {
        if (!m_window.resized())
        {
            return;
        }

        device.wait();
        m_swapChain.setWidth(m_window.getWidth()).setHeight(m_window.getHeight());
        m_swapChain.recreateSwapChain(device, m_surface);
    }

    void App::prepareForCurrentFrame()
    {
        m_window.prepareForCurrentFrame();
    }

    bool App::shouldClose() const
    {
        return m_window.shouldClose();
    }

    const TiRHI::SwapChain& App::getSwapChain() const
    {
        return m_swapChain;
    }

    TiRHI::SwapChain& App::getSwapChain()
    {
        return m_swapChain;
    }
} // Ti::Core::Plateform
