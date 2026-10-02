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
        glfwTerminate();
    }

    Window::Window(int witdh, int height, TiRHI::RHI& rhi, TiRHI::Device& device)
        : m_width(witdh)
        , m_height(height)
        , m_swapChain(rhi.newSwapChain())
    {
        glfwInit();
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
        // clang-format off
        m_swapChain.setWidth(m_width)
            .setHeight(m_height)
            .setVsync(true)
            .setName("SwapChain")
            .build(device,  glfwGetWin32Window(m_window));
        // clang-format on
    }

    bool Window::shouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::poolEvent()
    {
        glfwPollEvents();
    }

    bool Window::beginFrame()
    {
        return m_swapChain.beginFrame();
    }

    void Window::endFrame(TiRHI::Device& device)
    {
        m_swapChain.present(device);
    }
}
