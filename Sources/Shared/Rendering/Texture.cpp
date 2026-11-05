#include <Assets/Texture.hpp>
#include "Texture.hpp"

namespace Ti::Rendering
{
    Texture::Texture(TiRHI::RHI& rhi)
        : m_texture(rhi)
    {
    }

    Texture::Texture(TiRHI::Device& device, const Assets::Texture& assetsTexture)
        : m_texture(device.getRHI())
    {
        Assets::AssetWeakRef<const Assets::Image> weakref = assetsTexture.getImage();

        if (Assets::AssetStrongRef strongRef = weakref.lock())
        {
            // TODO get app and equeu render command
            // upload texture

            /*
            // Keep the image alive until upload
            enqueueRenderCommand(
                [image = std::move(strongRef)]()
                {
                    // Create GPU texture
                    // Upload image pixels
                });

            */

            m_texture.setWidth(strongRef->getSize())
                .setHeight(strongRef->getSize())
                .setName("TestTexture")
                .setType(TiRHI::TextureType::Texture2D)
                .setFlags(TiRHI::TextureFlags::CopyDestination | TiRHI::TextureFlags::ShaderResource)
                .build(device);
        }
    }
} // namespace Ti::Rendering
