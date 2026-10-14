#include <Io.hpp>

#include <GLFW/glfw3.h>

namespace Ti::Core::Io
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

} // Ti::Core::Io
