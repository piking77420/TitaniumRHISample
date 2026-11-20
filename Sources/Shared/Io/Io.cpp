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
    }

} // Ti::Io
