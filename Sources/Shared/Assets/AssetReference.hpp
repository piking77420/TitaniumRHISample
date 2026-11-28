#ifndef TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H
#define TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H

#include <atomic>
#include <memory>
#include <Assets/Asset.hpp>

namespace Ti::Assets
{
    template<typename T>
    using AssetPtr = std::shared_ptr<const T>;

    template<typename T>
    using AssetStrongRef = std::shared_ptr<T>;

    template<typename T>
    using AssetWeakRef = std::weak_ptr<T>;

    template<typename T>
    using AtomicAssetPtr = std::atomic<AssetPtr<T>>;

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_ASSETS_REFERENCE_H
