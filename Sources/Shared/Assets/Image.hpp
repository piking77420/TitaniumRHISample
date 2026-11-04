#ifndef TITANIUM_SAMPLE_ASSETS_IMAGE_H
#define TITANIUM_SAMPLE_ASSETS_IMAGE_H

#include <memory>
#include <string_view>
#include <filesystem>

namespace Ti::Assets
{
    class Image
    {
    public:
        Image() = default;
        ~Image() = default;
        Image(const std::filesystem::path path);

        operator bool() const
        {
            return m_data.get() != nullptr;
        }

        const std::unique_ptr<std::byte[]>& getData() const
        {
            return m_data;
        }

        size_t getSize() const
        {
            return m_size;
        }

        size_t getChannel() const
        {
            return m_size;
        }

    private:
        std::unique_ptr<std::byte[]> m_data;

        size_t m_size = 0;

        size_t m_channel = 0;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_IMAGE_H
