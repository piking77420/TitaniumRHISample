#ifndef TITATNIUM_SAMPLE_PLATEFORM_H
#define TITATNIUM_SAMPLE_PLATEFORM_H

#include <string_view>

#include <Titanium/TitaniumHeader.hpp>
#include <Titanium/RHI-DebugScope.hpp>
#include <Plateform/Window.hpp>

namespace Ti::Plateform
{
    class App
    {
    public:
        App();
        virtual ~App();
        App(std::string name);

    private:
        std::string m_name;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_H
