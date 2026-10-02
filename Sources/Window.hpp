#ifndef TI_SAMPLE_IO_WINDOW_H
#define TI_SAMPLE_IO_WINDOW_H

#include <Titanium/TitaniumHeader.hpp>
#include <GLFW/glfw3.h>

namespace TiSample::Io
{
    class Window
    {
    public:
        Window() = delete;
        ~Window();
        Window(int witdh, int height, TiRHI::RHI& rhi, TiRHI::Device& device);

        bool shouldClose();
        void poolEvent();

        bool beginFrame();
        void endFrame(TiRHI::Device& device);

    private:
        int m_width = 0;
        int m_height = 0;
        bool m_resized = false;
        GLFWwindow* m_window{nullptr};

        TiRHI::SwapChain m_swapChain;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
}

#endif // TI_SAMPLE_IO_WINDOW_H
