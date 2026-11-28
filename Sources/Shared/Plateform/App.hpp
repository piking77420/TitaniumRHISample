#ifndef TITATNIUM_SAMPLE_PLATEFORM_H
#define TITATNIUM_SAMPLE_PLATEFORM_H

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <Titanium/TitaniumHeader.hpp>

namespace Ti::Plateform
{
    using RhiCommand = std::move_only_function<void(TiRHI::Device&, TiRHI::CommandList&)>; // may use variant

    // TODO CREATE AND RHITHREAD CLASS
    // WILL BE MONOTHREAD IN EXEMPLES
    // AND ON OTHER THREAD IN the RENDER SAMPLE
    class App
    {
    public:
        App();
        virtual ~App();
        App(std::string name);

        std::string_view getName() const
        {
            return m_name;
        }

        void pushRhiCommand(RhiCommand rhiCommand)
        {
            m_rhiCommands.push_back(std::move(rhiCommand));
        }

    protected:
        // TODO make it producer consumer buffer
        // exemple main -> main
        // Future render main -> rhi
        std::vector<RhiCommand> m_rhiCommands;

    private:
        std::string m_name;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_H
