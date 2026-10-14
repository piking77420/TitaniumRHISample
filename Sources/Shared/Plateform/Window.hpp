#ifndef TITANIUM_SAMPLE_PLATEFROM_WINDOW_H
#define TITANIUM_SAMPLE_PLATEFROM_WINDOW_H

#include <GLFW/glfw3.h>
#include <Titanium/TitaniumHeader.hpp>

namespace Ti::Plateform
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

        bool shouldClose() const;
        void prepareForCurrentFrame();

        TiRHI::WindowHandle getWindowHandle() const;

    private:
        int m_width = 0;
        int m_height = 0;
        bool m_resized = false;
        GLFWwindow* m_window{nullptr};

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
} // Ti::Plateform

#endif // TITANIUM_SAMPLE_PLATEFROM_WINDOW_H
