#include <Plateform/BaseExemple.hpp>
#include <Core/Log.hpp>
#include <utility>
using namespace std::literals;
namespace Ti::Plateform
{
    BaseExemple::~BaseExemple()
    {
    }

    BaseExemple::BaseExemple(std::string name)
        : App(std::move(name))
        , m_window(static_cast<int>(WindowWidthBaseSize), static_cast<int>(WindowHeightBaseSize), getName().data())
        , m_rhiThread(m_window.getWindowHandle(), static_cast<uint32_t>(m_window.getWidth()),
                      static_cast<uint32_t>(m_window.getHeight()))
    {
        if (!m_imguiIntegration.build(getDevice(), getWindow(), getSwapChain()))
            TI_LOG_ERROR(Core, "Failed to Init Imgui");
    }

    void BaseExemple::handleResize()
    {
        if (!m_window.resized())
        {
            return;
        }
        // To avoid device wait we need to use deferred destroy
        // m_device.wait();
        const uint32_t width = static_cast<uint32_t>(m_window.getWidth());
        const uint32_t height = static_cast<uint32_t>(m_window.getHeight());
        m_rhiThread.pushRhiCommand(
            [width, height](Rendering::RhiThread& thread)
            {
                if (!thread.getSwapChain().setWidth(width).setHeight(height).recreateSwapChain(thread.getSurface()))
                    TI_LOG_ERROR(RHI, "Failed to recreate swap chain after resize");
            });
        m_rhiThread.consumeCommands();
        onResizeWindow();
    }

    void BaseExemple::record(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        if (!cmd.beginRecord())
            return;

        m_rhiThread.consumeOnRecordRhiCommands();

        onRecord(cmd, swapChain);

        {
            TiRHI::DebugScope _(cmd, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
            TiRHI::RenderTargets& renderTarget = swapChain.getCurrentRenderTargets();

            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(swapChain.getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;
            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor);

            cmd.transitionResource(swapChain.getCurrentSwapChainTexture(), TiRHI::ResourceState::RenderTarget);
            if (cmd.beginRenderPass(renderPass, renderTarget))
            {
                onSwapChainRenderPass(cmd, swapChain);

                {
                    TiRHI::DebugScope _(cmd, "Imgui Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
                    m_imguiIntegration.record(cmd);
                }

                cmd.endRenderPass();
            }
            cmd.transitionResource(swapChain.getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
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
                m_rhiThread.consumeCommands();
                TiRHI::Device& device = getDevice();
                TiRHI::SwapChain& swapChain = getSwapChain();
                TiRHI::CommandList& commandList = m_rhiThread.getCommandList();

                device.beginFrame();
                render();

                std::array frames = {swapChain.acquireNextImage()};

                if (frames[0].getSucces())
                {
                    record(commandList, swapChain);

                    const std::array submitDesc = {
                        TiRHI::Device::SubmitPresentDesc{.commandList = &commandList, .acquireFrames = frames}};

                    device.submitPresent(submitDesc);
                    getRHI().nextFrame();
                }
            }
        }
        getDevice().wait();
    }

} // Ti::Core::Platerform
