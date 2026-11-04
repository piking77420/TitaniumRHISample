#include <Assets/Texture.hpp>

namespace Ti::Assets
{
    Texture::Texture(const AssetStrongRef<const Image>& image)
        : m_image(image)
    {
    }
} // namespace Ti::Assets
