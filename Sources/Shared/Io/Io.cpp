#include <Io/Io.hpp>

#include <GLFW/glfw3.h>

namespace Ti::Io
{
    Io::Io()
    {
        glfwInit();
    }

    Io::~Io()
    {
        glfwTerminate();
    }

    void Io::poolEvents()
    {
        glfwPollEvents();
        const double currentTime = glfwGetTime();
        m_deltatime = m_time != 0.0 ? currentTime - m_time : 0.0;
        m_time = currentTime;
    }

} // Ti::Io
