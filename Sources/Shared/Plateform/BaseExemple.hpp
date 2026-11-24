#ifndef TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H
#define TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H

#include <string_view>
#include <tuple>

#include <Titanium/TitaniumHeader.hpp>
#include <Titanium/RHI-DebugScope.hpp>
#include <Plateform/Window.hpp>
#include <Plateform/App.hpp>
#include <Io/Io.hpp>
#include <ImGuiIntegration/ImGuiIntegration.hpp>*

namespace Ti::Plateform
{
    constexpr size_t WindowWidthBaseSize = 1280;
    constexpr size_t WindowHeightBaseSize = 720;

    class BaseExemple : public App
    {
    public:
        BaseExemple() = delete;
        ~BaseExemple();

        BaseExemple(std::string name);

        void run();

    protected:
        void handleResize();

        void render(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain);

        void update();

        virtual void onRender(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) = 0;

        virtual void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) = 0;

        virtual void onUpdate() = 0;

        virtual void onResizeWindow() {};

        TiRHI::RHI& getRHI()
        {
            return m_rhi;
        }

        const TiRHI::RHI& getRHI() const
        {
            return m_rhi;
        }

        TiRHI::Device& getDevice()
        {
            return m_device;
        }

        TiRHI::SwapChain& getSwapChain()
        {
            return m_swapChain;
        }

        const TiRHI::SwapChain& getSwapChain() const
        {
            return m_swapChain;
        }

        Ti::Plateform::Window& getWindow()
        {
            return m_window;
        }

        void pushRenderCommand(std::move_only_function<void(TiRHI::CommandList&)>&& func)
        {
            m_pendingRenderCommand.emplace_back(std::move(func));
        }

        static constexpr size_t ImageCount = 3;

        static constexpr std::array AppClearColor{0.1f, 0.2f, 0.4f, 1.0f};

    private:
        Ti::Io::Io m_io;

        Ti::Plateform::Window m_window;

        TiRHI::RHI m_rhi;

        TiRHI::Device m_device;

        TiRHI::Surface m_surface;

        TiRHI::SwapChain m_swapChain;

        TiRHI::CommandList m_cmdList;

        Ti::ThirdParty::ImguiIntegration m_imguiIntegration;

        std::vector<std::move_only_function<void(TiRHI::CommandList&)>> m_pendingRenderCommand;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H
