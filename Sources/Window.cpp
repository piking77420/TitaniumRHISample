#include <Window.hpp>

#include <GLFW/glfw3.h>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace TiSample::Io
{
    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* data = static_cast<Window*>(glfwGetWindowUserPointer(window));
        data->m_resized = true;
        data->m_width = width;
        data->m_height = height;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
    }

    Window::Window(int witdh, int height)
        : m_width(witdh)
        , m_height(height)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_width, height, "TitaniumRHISample", nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::prepareForCurrentFrame()
    {
        m_resized = false;
    }

    TiRHI::WindowHandle Window::getWindowHandle() const
    {
        return TiRHI::WindowHandle(glfwGetWin32Window(m_window));
    }
}
