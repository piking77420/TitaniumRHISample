#ifndef TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H
#define TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H

#include <string_view>
#include <tuple>
#include <utility>

#include <Titanium/TitaniumHeader.hpp>
#include <Titanium/RHI-DebugScope.hpp>
#include <Plateform/Window.hpp>
#include <Plateform/App.hpp>
#include <Io/Io.hpp>
#include <ImGuiIntegration/ImGuiIntegration.hpp>
#include <Rendering/RhiThread.hpp>

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

        void update();

        void render();

        void record(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain);

        virtual void onResizeWindow() {};

        virtual void onUpdate() = 0;

        virtual void onRender() = 0;

        virtual void onRecord(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) = 0;

        virtual void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) = 0;

        const Ti::Io::Io& getIo() const
        {
            return m_io;
        }

        Ti::Plateform::Window& getWindow()
        {
            return m_window;
        }

        Ti::Rendering::RhiThread& getRhiThread()
        {
            return m_rhiThread;
        }

        TiRHI::RHI& getRHI()
        {
            return m_rhiThread.getRHI();
        }

        TiRHI::Device& getDevice()
        {
            return m_rhiThread.getDevice();
        }

        TiRHI::SwapChain& getSwapChain()
        {
            return m_rhiThread.getSwapChain();
        }

        void pushRhiCommand(Rendering::RhiCommand command)
        {
            m_rhiThread.pushRhiCommand(std::move(command));
        }

        void pushRhiOnRecordCommand(Rendering::RhiOnRecordCommand command)
        {
            m_rhiThread.pushOnRecordCommand(std::move(command));
        }

        static constexpr size_t ImageCount = 3;

        static constexpr std::array AppClearColor{0.1f, 0.2f, 0.4f, 1.0f};

    private:
        Ti::Io::Io m_io;

        Ti::Plateform::Window m_window;

        // Rhi Thread is not in the upper class
        // because exemple should be basic and dont use multithread
        // however to keep the api consistant we use it here as mono thread
        // with this each module can use Ti::Rendering::RhiThread
        // important to use module in the future render(multithread) and the exemples
        Ti::Rendering::RhiThread m_rhiThread;

        Ti::ThirdParty::ImguiIntegration m_imguiIntegration;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_BASE_EXEMPLE_H
