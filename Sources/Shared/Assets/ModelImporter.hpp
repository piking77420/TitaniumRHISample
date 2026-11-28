#ifndef TITANIUM_SAMPLE_ASSETS_MESH_IMPORTER_H
#define TITANIUM_SAMPLE_ASSETS_MESH_IMPORTER_H

#include <filesystem>
#include <Assets/AssetReference.hpp>

namespace Ti::Plateform
{
    class App;
}

namespace Ti::Assets
{
    class StaticMesh;

    class ModelImporter
    {
    public:
        ModelImporter() = delete;
        ~ModelImporter() = default;
        ModelImporter(const std::filesystem::path& path);

        AssetPtr<StaticMesh> makeStaticMesh(Plateform::App& app);

    private:
        std::filesystem::path m_path;
    };

} // namespace Ti::Assets

#endif // TITANIUM_SAMPLE_ASSETS_MESH_IMPORTER_H
