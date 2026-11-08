#ifndef TITANIUM_SAMPLE_ASSETS_IMAGE_H
#define TITANIUM_SAMPLE_ASSETS_IMAGE_H

#include <memory>
#include <string_view>
#include <filesystem>

#include <Assets/Asset.hpp>

namespace Ti::Assets
{
    class Image : public Asset
    {
    public:
        Image();
        virtual ~Image();
        Image(const std::filesystem::path& path);

        operator bool() const
        {
            return m_data.get() != nullptr;
        }

        const std::byte* getData() const noexcept
        {
            return m_data.get();
        }

        std::unique_ptr<std::byte[]> acquireData() && noexcept
        {
            m_width = 0;
            m_height = 0;
            m_totalSize = 0;
            m_channel = 0;

            return std::move(m_data);
        }

        size_t getTotalSize() const
        {
            return m_totalSize;
        }
        size_t getChannel() const
        {
            return m_channel;
        }
        size_t getWidth() const
        {
            return m_width;
        }
        size_t getHeight() const
        {
            return m_height;
        }

    private:
        std::unique_ptr<std::byte[]> m_data;

        size_t m_width = 0;

        size_t m_height = 0;

        size_t m_totalSize = 0;

        size_t m_channel = 0;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_IMAGE_H
