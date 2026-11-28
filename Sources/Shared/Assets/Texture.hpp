#ifndef TITANIUM_SAMPLE_ASSETS_TEXTURE_H
#define TITANIUM_SAMPLE_ASSETS_TEXTURE_H

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
        explicit Texture(AssetStrongRef<const Image> image);

        const AssetWeakRef<const Image>& getImage() const noexcept
        {
            return m_image;
        }

    private:
        AssetWeakRef<const Image> m_image;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_TEXTURE_H
