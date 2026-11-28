#include <Assets/ModelImporter.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Assets/StaticMesh.hpp>
#include <Assets/MeshSection.hpp>
#include <limits>
#include <vector>
#include <Vector3.hpp>
#include <Vector2.hpp>

namespace Ti::Assets
{
    namespace
    {
        void generateMeshSectionFromAssimpNode(const aiNode& node, const std::vector<MeshSection>& sectionsByMesh,
                                               std::vector<MeshSection>& meshSections)
        {
            for (uint32_t i = 0; i < node.mNumMeshes; ++i)
                meshSections.push_back(sectionsByMesh.at(node.mMeshes[i]));

            for (uint32_t i = 0; i < node.mNumChildren; ++i)
                generateMeshSectionFromAssimpNode(*node.mChildren[i], sectionsByMesh, meshSections);
        }
    }

    ModelImporter::ModelImporter(const std::filesystem::path& path)
        : m_path(path)
    {
    }

    AssetPtr<StaticMesh> ModelImporter::makeStaticMesh(Rendering::RhiThread& rhiThread)
    {
        if (!std::filesystem::exists(m_path))
            return {};

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(
            m_path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
                                 aiProcess_ImproveCacheLocality | aiProcess_PreTransformVertices);

        if (!scene || !scene->HasMeshes())
            return {};

        std::vector<Rendering::StaticMesh::Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<MeshSection> sectionsByMesh(scene->mNumMeshes);

        for (uint32_t m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];

            if (vertices.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max()) ||
                indices.size() > std::numeric_limits<uint32_t>::max())
            {
                return {};
            }

            const uint32_t firstIndex = static_cast<uint32_t>(indices.size());
            const int32_t vertexOffset = static_cast<int32_t>(vertices.size());

            // Vertices
            for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
            {
                Rendering::StaticMesh::Vertex vertex;

                const aiVector3D& position = mesh->mVertices[i];

                vertex.position = MathLib::Vector3f{position.x, position.y, position.z};

                if (mesh->HasNormals())
                {
                    const aiVector3D& normal = mesh->mNormals[i];

                    vertex.normal = MathLib::Vector3f{normal.x, normal.y, normal.z};
                }

                if (mesh->HasTextureCoords(0))
                {
                    const aiVector3D& uv = mesh->mTextureCoords[0][i];

                    vertex.uv = MathLib::Vector2f{uv.x, uv.y};
                }

                vertices.push_back(vertex);
            }

            // Indices
            for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
            {
                const aiFace& face = mesh->mFaces[i];

                if (face.mNumIndices != 3)
                    continue;

                for (uint32_t j = 0; j < face.mNumIndices; ++j)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            if (vertices.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max()) ||
                indices.size() > std::numeric_limits<uint32_t>::max())
            {
                return {};
            }

            sectionsByMesh[m] = MeshSection{
                firstIndex,
                static_cast<uint32_t>(indices.size() - firstIndex),
                vertexOffset,
                mesh->mMaterialIndex,
            };
        }

        if (!scene->mRootNode)
        {
            return {};
        }

        std::vector<MeshSection> meshSections;
        generateMeshSectionFromAssimpNode(*scene->mRootNode, sectionsByMesh, meshSections);

        return std::make_shared<StaticMesh>(rhiThread, m_path.stem().string(), std::move(vertices), std::move(indices),
                                            std::move(meshSections));
    }

} // namespace Ti::Assets
