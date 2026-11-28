#ifndef TITANIUM_SAMPLE_ASSETS_ASSETS_H
#define TITANIUM_SAMPLE_ASSETS_ASSETS_H

#include <memory>
#include <string>
#include <string_view>

namespace Ti::Assets
{
    class Asset
    {
    public:
        Asset();
        virtual ~Asset();
        Asset(std::string name)
            : m_name(name)
        {
        }

        std::string_view getName() const
        {
            return m_name;
        }

    private:
        std::string m_name;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_ASSETS_H
