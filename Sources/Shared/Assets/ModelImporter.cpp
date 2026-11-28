#include <Assets/ModelImporter.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Assets/StaticMesh.hpp>
#include <Vector3.hpp>
#include <Vector2.hpp>
#include <Matrix4x4.hpp>

namespace Ti::Assets
{
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

        for (uint32_t m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];

            const uint32_t vertexOffset = static_cast<uint32_t>(vertices.size());

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
                    indices.push_back(vertexOffset + face.mIndices[j]);
                }
            }
        }

        return std::make_shared<StaticMesh>(rhiThread, m_path.stem().string(), std::move(vertices),
                                            std::move(indices));
    }

} // namespace Ti::Assets
