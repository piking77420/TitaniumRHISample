#ifndef TITANIUM_SAMPLE_RENDERING_TEXTURE_H
#define TITANIUM_SAMPLE_RENDERING_TEXTURE_H

#include <memory>
#include <string_view>
#include <filesystem>

#include <Titanium/TitaniumHeader.hpp>
#include <Rendering/GpuResource.hpp>

namespace Ti::Assets
{
    class Texture;
} // namespace Ti::Assets

namespace Ti::Rendering
{

    class Texture : public GpuResource
    {
    public:
        Texture(TiRHI::RHI& rhi);
        virtual ~Texture() = default;
        Texture(TiRHI::RHI& rhi, const Assets::Texture& assetsTexture);

        TiRHI::Texture& getRhiTexture()
        {
            return m_texture;
        }

        bool isValid()
        {
            return m_texture.isValid();
        }

    private:
        TiRHI::Texture m_texture;
    };

} // namespace Ti::Rendering

#endif // TITANIUM_SAMPLE_RENDERING_TEXTURE_H
