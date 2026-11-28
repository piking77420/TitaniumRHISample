#include <Assets/Texture.hpp>

#include <utility>

namespace Ti::Assets
{
    Texture::Texture(AssetStrongRef<const Image> image)
        : m_image(std::move(image))
    {
    }
} // namespace Ti::Assets
