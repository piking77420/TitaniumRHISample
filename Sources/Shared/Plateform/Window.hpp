#ifndef TITANIUM_SAMPLE_PLATEFROM_WINDOW_H
#define TITANIUM_SAMPLE_PLATEFROM_WINDOW_H

#include <array>
#include <GLFW/glfw3.h>
#include <Titanium/TitaniumHeader.hpp>

namespace Ti::Plateform
{
    template<typename T>
    requires(std::is_enum_v<T> && requires { T::Count; })
    class InputState
    {
    public:
        void beginFrame()
        {
            for (auto& state : m_states)
            {
                state.pressed = false;
                state.released = false;
            }
        }

        // isPressed true => pressed
        // isPressed false => release
        void setKeyOnPressOrRelease(T key, bool isPressed)
        {
            auto& state = m_states[static_cast<size_t>(key)];

            if (isPressed)
            {
                state.down = true;
                state.pressed = true;
            }
            else
            {
                state.down = false;
                state.released = true;
            }
        }

        bool isDown(T key) const
        {
            return m_states[static_cast<size_t>(key)].down;
        }

        bool isPressed(T key) const
        {
            return m_states[static_cast<size_t>(key)].pressed;
        }

        bool isReleased(T key) const
        {
            return m_states[static_cast<size_t>(key)].released;
        }

    private:
        struct KeyState
        {
            bool down = false;
            bool pressed = false;
            bool released = false;
        };

        std::array<KeyState, static_cast<size_t>(T::Count)> m_states{};
    };

    struct Mouse
    {
        double mouseMovementDx = 0.0;
        double mouseMovementDy = 0.0;
        double scrollDx = 0.0;
        double scrollDy = 0.0;

        enum struct Key
        {
            Left,
            Middle,
            Right,
            Count,
        };

        void beginFrame()
        {
            mouseMovementDx = 0.0;
            mouseMovementDy = 0.0;
            scrollDx = 0.0;
            scrollDy = 0.0;
            inputState.beginFrame();
        }

        InputState<Key> inputState;
    };

    struct KeyBoard
    {
        enum struct Key : uint32_t
        {
            W,
            S,
            D,
            A,
            E,
            Q,
            Shift,
            Count,
        };

        InputState<Key> inputState;
    };

    enum struct CursorState
    {
        Visible,
        Hide,
        Disable,
        Capture
    };

    class Window
    {
    public:
        Window() = default;
        ~Window();
        Window(int witdh, int height, const char* windowName);

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

        GLFWwindow* getGlfwWindow() const
        {
            return m_window;
        }

        const Mouse& getMouse() const
        {
            return m_mouse;
        }

        const KeyBoard& getKeyBoard() const
        {
            return m_keyBoard;
        }

        TiRHI::WindowHandle getWindowHandle() const;

        const Window& setCursorState(CursorState state);

        CursorState getCursorState() const
        {
            return m_cursorState;
        }

    private:
        int m_width = 0;
        int m_height = 0;
        bool m_resized = false;
        GLFWwindow* m_window{nullptr};
        bool m_firstMouse{false};
        Mouse m_mouse;
        KeyBoard m_keyBoard;
        CursorState m_cursorState = CursorState::Visible;
        double m_mouseCursorLastX = 0.0;
        double m_mouseCursorLastY = 0.0;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
} // Ti::Plateform

#endif // TITANIUM_SAMPLE_PLATEFROM_WINDOW_H
