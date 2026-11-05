#include <Assets/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ti::Assets
{
    Image::Image() = default;

    Image::~Image() = default;

    Image::Image(const std::filesystem::path path)
    {
        if (!std::filesystem::exists(path)) // TODO do log
            return;

        const std::string s = path.generic_string();

        if (s.empty())
            return;

        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_uc* data = stbi_load(s.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!data)
            return;

        m_channel = static_cast<size_t>(channels);
        m_size = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);
        m_data.reset(new std::byte[m_size]);
        std::memcpy(m_data.get(), data, m_size);
        stbi_image_free(data);
    }
} // namespace Ti::Assets
