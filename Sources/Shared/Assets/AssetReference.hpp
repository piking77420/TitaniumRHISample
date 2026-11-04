#ifndef TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H
#define TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H

#include <memory>
#include <Assets/Asset.hpp>

namespace Ti::Assets
{
    template<typename T>
    concept AssetType = std::derived_from<T, Asset>;

    template<AssetType T>
    using AssetStrongRef = std::shared_ptr<T>;

    template<AssetType T>
    using AssetWeakRef = std::weak_ptr<T>;

    template<AssetType T>
    using AtomicAssetStrongRef = std::atomic<AssetStrongRef<T>>;

    template<AssetType T>
    using AtomicAssetWeakRef = std::atomic<AssetWeakRef<T>>;

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H
