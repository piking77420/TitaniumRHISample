#include <Assets/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Io/File.hpp>

namespace Ti::Assets
{
    Image::Image() = default;

    Image::~Image() = default;

    Image::Image(const std::filesystem::path& path)
    {
        std::unique_ptr<std::byte[]> fileData;
        uint64_t fileSize = 0;

        {
            Io::File file(path, Io::File::OpenMode::Read);
            if (!file.isOpen())
                return;

            fileSize = file.getSize();

            if (fileSize == 0 || fileSize > static_cast<uint64_t>(std::numeric_limits<int>::max()))
            {
                return;
            }

            fileData = file.clone();
        }

        if (!fileData)
            return;

        int width = 0;
        int height = 0;
        int sourceChannels = 0;

        stbi_uc* pixels =
            stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(fileData.get()), static_cast<int>(fileSize), &width,
                                  &height, &sourceChannels, STBI_rgb_alpha);

        if (!pixels)
            return;

        m_width = static_cast<size_t>(width);
        m_height = static_cast<size_t>(height);
        m_channel = STBI_rgb_alpha;
        m_totalSize = m_width * m_height * m_channel;

        m_data = std::make_unique<std::byte[]>(m_totalSize);
        std::memcpy(m_data.get(), pixels, m_totalSize);

        stbi_image_free(pixels);
    }
} // namespace Ti::Assets
