#ifndef TITATNIUM_SAMPLE_PLATEFORM_H
#define TITATNIUM_SAMPLE_PLATEFORM_H

#include <string>
#include <string_view>

namespace Ti::Plateform
{
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

    private:
        std::string m_name;
    };

} // Ti::Core::Platerform

#endif // !TITATNIUM_SAMPLE_PLATEFORM_H
