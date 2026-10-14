#ifndef TITATNIUM_SAMPLE_PLATEFORM_H
#define TITATNIUM_SAMPLE_PLATEFORM_H

#include <string_view>

#include <Titanium/TitaniumHeader.hpp>
#include <Titanium/RHI-DebugScope.hpp>
#include <Window.hpp>

namespace Ti::Plateform
{
    constexpr size_t WindowWidthBaseSize = 1280;
    constexpr size_t WindowHeightBaseSize = 720;
    constexpr std::array AppClearColor{0.1f, 0.2f, 0.4f, 1.0f};
    constexpr size_t imageCount = 3;

    class App
    {
    public:
        App() = delete;
        App(TiRHI::RHI& rhi, TiRHI::Device& device);
        ~App();

        TiRHI::GraphicPipeline* graphicPipeline = nullptr;

        std::pair<TiRHI::AcquiredFrame, TiRHI::CommandList*> render(TiRHI::Device& device);

        void swapBuffer();

        void handleResize(TiRHI::Device& device);

        void prepareForCurrentFrame();

        bool shouldClose() const;

        const TiRHI::SwapChain& getSwapChain() const;

        TiRHI::SwapChain& getSwapChain();

    private:
        Ti::Plateform::Window m_window;

        TiRHI::Surface m_surface;

        TiRHI::SwapChain m_swapChain;

        TiRHI::CommandList m_cmdList;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_H
