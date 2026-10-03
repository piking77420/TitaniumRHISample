#ifndef TI_SAMPLE_IO_WINDOW_H
#define TI_SAMPLE_IO_WINDOW_H

#include <GLFW/glfw3.h>
#include <Titanium/TitaniumHeader.hpp>

namespace TiSample::Io
{
    class Window
    {
    public:
        Window() = delete;
        ~Window();
        Window(int witdh, int height);

        int getWidth() const
        {
            return m_width;
        }

        int getHeight() const
        {
            return m_height;
        }

        bool resized() const
        {
            return m_resized;
        }

        bool shouldClose();
        void pollEvents();

        TiRHI::WindowHandle getWindowHandle() const;

    private:
        int m_width = 0;
        int m_height = 0;
        bool m_resized = false;
        GLFWwindow* m_window{nullptr};

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
}

#endif // TI_SAMPLE_IO_WINDOW_H
