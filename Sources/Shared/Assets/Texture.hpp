#ifndef TITANIUM_SAMPLE_RENDERING_ASSETS_H
#define TITANIUM_SAMPLE_RENDERING_ASSETS_H

#include <string_view>
#include <filesystem>

#include <Assets/AssetReference.hpp>
#include <Assets/Image.hpp>

namespace Ti::Assets
{
    class Texture
    {
    public:
        Texture() = default;
        ~Texture() = default;
        Texture(AssetStrongRef<const Image> image);

        const AssetWeakRef<const Image>& getImage() const
        {
            return m_image;
        }

    private:
        AssetWeakRef<const Image> m_image;
    };

} // namespace Ti::Rendering

#endif // TITANIUM_SAMPLE_RENDERING_ASSETS_H
