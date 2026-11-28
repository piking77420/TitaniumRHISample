#include <Rendering/Texture.hpp>
#include <Assets/Texture.hpp>

namespace Ti::Rendering
{
    Texture::Texture(TiRHI::Device& device)
        : m_texture(device)
    {
    }

    Texture::Texture(TiRHI::Device& device, const Assets::Texture& assetsTexture)
        : m_texture(device)
    {
    }
} // namespace Ti::Rendering
