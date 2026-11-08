#include <Plateform/App.hpp>
#include <Core/Log.hpp>
#include <Core/Thread.hpp>

namespace Ti::Plateform
{
    using namespace std::literals;

    App::App() = default;

    App::~App() = default;

    App::App(std::string name)
        : m_name(name)
    {
        Ti::Core::SetThreadName("PlateFrom Thread");
        Ti::Core::Log::setFileOutPut("Saved/Log.txt");
    }

} // Ti::Core::Plateform
