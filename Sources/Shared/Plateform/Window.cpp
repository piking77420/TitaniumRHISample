#include <Plateform/Window.hpp>

#include <GLFW/glfw3.h>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <Core/Log.hpp>

namespace Ti::Plateform
{
    std::optional<bool> getInputStateIsPressed(int glfwKey)
    {
        if (glfwKey == GLFW_PRESS)
        {
            return true;
        }
        else if (glfwKey == GLFW_RELEASE)
        {
            return false;
        }

        return {};
    }

    [[nodiscard]] std::optional<KeyBoard::Key> glfwKeyToKeyBoard(int glfwKey)
    {
        using Key = KeyBoard::Key;

        switch (glfwKey)
        {
        case GLFW_KEY_W:
            return Key::W;
        case GLFW_KEY_S:
            return Key::S;
        case GLFW_KEY_D:
            return Key::D;
        case GLFW_KEY_A:
            return Key::A;
        case GLFW_KEY_E:
            return Key::E;
        case GLFW_KEY_Q:
            return Key::Q;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            return Key::Shift;

        default:
            TI_LOG_WARNING(Plateform, "Not supported key keyboard {}", glfwGetKeyName(GLFW_KEY_W, 0));
            return std::nullopt;
        }
    }

    [[nodiscard]] std::optional<Mouse::Key> glfwKeyToKeyMouse(int glfwKey)
    {
        using Key = Mouse::Key;

        switch (glfwKey)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return Key::Left;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return Key::Right;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return Key::Right;

        default:
            TI_LOG_WARNING(Plateform, "Unsupported mouse button {}", glfwKey);

            return std::nullopt;
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* data = static_cast<Window*>(glfwGetWindowUserPointer(window));
        data->m_resized = true;
        data->m_width = width;
        data->m_height = height;
    }

    Window::~Window()
    {
        if (m_window != nullptr)
            glfwDestroyWindow(m_window);
    }

    Window::Window(int witdh, int height, const char* windowName)
        : m_width(witdh)
        , m_height(height)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_width, m_height, windowName, nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }
        // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
        glfwSetCursorPosCallback(m_window,
                                 [](GLFWwindow* window, double x, double y)
                                 {
                                     Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
                                     if (!w)
                                         return;
                                     auto& mouse = w->m_mouse;
                                     if (!w->m_firstMouse)
                                     {
                                         w->m_mouseCursorLastX = x;
                                         w->m_mouseCursorLastY = y;
                                         w->m_firstMouse = true;
                                     }
                                     mouse.mouseMovementDx += x - w->m_mouseCursorLastX;
                                     mouse.mouseMovementDy += w->m_mouseCursorLastY - y;
                                     w->m_mouseCursorLastX = x;
                                     w->m_mouseCursorLastY = y;
                                 });
        glfwSetKeyCallback(m_window,
                           [](GLFWwindow* window, int key, int scancode, int action, int mods)
                           {
                               Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
                               if (std::optional<KeyBoard::Key> keyKeyBoard = glfwKeyToKeyBoard(key))
                                   if (std::optional<bool> state = getInputStateIsPressed(action))
                                       w->m_keyBoard.inputState.setKeyOnPressOrRelease(*keyKeyBoard, *state);
                           });

        glfwSetMouseButtonCallback(m_window,
                                   [](GLFWwindow* window, int button, int action, int mods)
                                   {
                                       Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
                                       if (std::optional<Mouse::Key> key = glfwKeyToKeyMouse(button))
                                           if (std::optional<bool> state = getInputStateIsPressed(action))
                                               w->m_mouse.inputState.setKeyOnPressOrRelease(*key, *state);
                                   });
        glfwSetScrollCallback(m_window,
                              [](GLFWwindow* window, double xoffset, double yoffset)
                              {
                                  // yoffset > 0 : Scroll up
                                  // yoffset < 0 : Scroll down

                                  // xoffset : Horizontal scrolling
                                  Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
                                  w->m_mouse.scrollDx = xoffset;
                                  w->m_mouse.scrollDy = yoffset;
                              });
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::prepareForCurrentFrame()
    {
        // should be called befor pool event

        m_resized = false;
        m_mouse.beginFrame();
        m_keyBoard.inputState.beginFrame();
    }

    TiRHI::WindowHandle Window::getWindowHandle() const
    {
        return TiRHI::WindowHandle(glfwGetWin32Window(m_window));
    }

    const Window& Window::setCursorState(CursorState state)
    {
        int glfwCursorMode = 0;
        switch (state)
        {
        case CursorState::Visible:
            glfwCursorMode = GLFW_CURSOR_NORMAL;
            break;
        case CursorState::Hide:
            glfwCursorMode = GLFW_CURSOR_HIDDEN;
            break;
        case CursorState::Disable:
            glfwCursorMode = GLFW_CURSOR_DISABLED;
            break;
        case CursorState::Capture:
            glfwCursorMode = GLFW_CURSOR_CAPTURED;
            break;
        default:
            TI_LOG_ERROR(Plateform, "Invalid CursorState enum");
            return *this;
        }
        m_cursorState = state;
        glfwSetInputMode(m_window, GLFW_CURSOR, glfwCursorMode);
        return *this;
    }

} // Ti::Plateform
