#include <Assets/Texture.hpp>

namespace Ti::Assets
{
    Texture::Texture(AssetStrongRef<const Image> image)
        : m_image(image)
    {
    }
} // namespace Ti::Assets
