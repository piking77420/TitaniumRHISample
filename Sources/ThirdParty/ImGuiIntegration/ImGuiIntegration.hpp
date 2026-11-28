#ifndef TITANIUM_IMGUI_INTEGRATION_H
#define TITANIUM_IMGUI_INTEGRATION_H

#include <Titanium/TitaniumHeader.hpp>

namespace Ti::Plateform
{
    class Window;
} // namespace Plateform

namespace Ti::ThirdParty
{
    class ImguiIntegration
    {
    public:
        ImguiIntegration() = default;
        ~ImguiIntegration();

        bool build(const TiRHI::Device& device, Plateform::Window& window, const TiRHI::SwapChain& swapchain);

        void newFrame();

        void update();

        void record(TiRHI::CommandList& commandList);

    private:
        bool imguiVulkan(const Plateform::Window& window, const TiRHI::SwapChain& swapchain);

        bool imguiD3D12(const Plateform::Window& window, const TiRHI::SwapChain& swapchain);

        const TiRHI::Device* m_device = nullptr;
    };

}

#endif // TITANIUM_IMGUI_INTEGRATION_H
