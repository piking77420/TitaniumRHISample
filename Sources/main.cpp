#include <iostream>

#include <GLFW/glfw3.h>

constexpr size_t WindowWidthBaseSize = 1280;
constexpr size_t WindowHeightBaseSize = 720;

struct WindowData
{
    bool resized = false;
    int width = 0;
    int height = 0;
};

void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    data->resized = true;
    data->width = width;
    data->height = height;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    WindowData windowData;

    windowData.width = static_cast<int>(WindowWidthBaseSize);
    windowData.height = static_cast<int>(WindowWidthBaseSize);

    GLFWwindow* window = glfwCreateWindow(windowData.width, windowData.height, "TitaniumRHISample", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, &windowData);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
