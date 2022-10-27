#include "render/Model.hpp"

#include "core/Logger.hpp"
#include "ecs/Entity.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Birdy3d::render {

    Model::Model(std::string const& path)
    {
        core::Logger::debug("Loading model: {}", path);
        load(path);
        compute_bounding_box();
    }

    Model::Model(Mesh mesh)
    {
        m_meshes.push_back(std::move(mesh));
        compute_bounding_box();
    }

    Model::Model(std::vector<Mesh>& meshes)
    {
        for (auto& mesh : meshes)
            m_meshes.push_back(std::move(mesh));
    }

    void Model::render(ecs::Entity& entity, Material const* material, Shader const& shader, bool transparent) const
    {
        if (material == nullptr)
            material = &m_embedded_material;
        glm::mat4 model = entity.transform.global_matrix();
        shader.set_mat4("model", model);
        for (auto const& m : m_meshes) {
            if (transparent == material->transparent())
                m.render(shader, *material);
        }
    }

    void Model::render_depth(ecs::Entity& entity, Shader const& shader) const
    {
        glm::mat4 model = entity.transform.global_matrix();
        shader.use();
        shader.set_mat4("model", model);
        for (auto const& m : m_meshes) {
            m.render_depth();
        }
    }

    void Model::render_wireframe(ecs::Entity& entity, Shader const& shader) const
    {
        glm::mat4 model = entity.transform.global_matrix();
        shader.use();
        shader.set_mat4("model", model);
        for (auto const& m : m_meshes) {
            m.render_wireframe();
        }
    }

    std::vector<Mesh> const& Model::get_meshes() const
    {
        return m_meshes;
    }

    void Model::load(std::string path)
    {
        Assimp::Importer importer;
        aiScene const* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            core::Logger::critical("ASSIMP error: {}", importer.GetErrorString());
            return;
        }
        m_directory = path.substr(0, path.find_last_of('/'));

        process_node(scene->mRootNode, scene);
    }

    void Model::process_node(aiNode* node, aiScene const* scene)
    {
        // process own meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(process_mesh(mesh, scene));
        }
        // children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node(node->mChildren[i], scene);
        }
    }

    Mesh Model::process_mesh(aiMesh* mesh, aiScene const* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        // process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            if (mesh->HasPositions())
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            else
                vertex.position = glm::vec3(0);

            if (mesh->HasTextureCoords(0))
                vertex.tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.tex_coords = glm::vec2(0);

            if (mesh->HasNormals())
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            else
                vertex.normal = glm::vec3(0);

            if (mesh->HasTangentsAndBitangents())
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            else
                vertex.tangent = glm::vec3(0);

            vertices.push_back(vertex);
        }

        // process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process material
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString path;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
            m_embedded_material.diffuse_map(m_directory + "/" + path.C_Str());
            m_embedded_material.diffuse_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            material->GetTexture(aiTextureType_SPECULAR, 0, &path);
            m_embedded_material.specular_map(m_directory + "/" + path.C_Str());
            m_embedded_material.specular_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
            material->GetTexture(aiTextureType_NORMALS, 0, &path);
            m_embedded_material.normal_map(m_directory + "/" + path.C_Str());
            m_embedded_material.normal_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
            material->GetTexture(aiTextureType_EMISSIVE, 0, &path);
            m_embedded_material.emissive_map(m_directory + "/" + path.C_Str());
            m_embedded_material.emissive_map_enabled = true;
        }

        return Mesh{vertices, indices};
    }

    void Model::compute_bounding_box()
    {
        glm::vec3 low(std::numeric_limits<float>::infinity());
        glm::vec3 high(-std::numeric_limits<float>::infinity());
        for (auto const& mesh : m_meshes) {
            for (Vertex vertex : mesh.vertices) {
                if (vertex.position.x < low.x)
                    low.x = vertex.position.x;
                if (vertex.position.y < low.y)
                    low.y = vertex.position.y;
                if (vertex.position.z < low.z)
                    low.z = vertex.position.z;
                if (vertex.position.x > high.x)
                    high.x = vertex.position.x;
                if (vertex.position.y > high.y)
                    high.y = vertex.position.y;
                if (vertex.position.z > high.z)
                    high.z = vertex.position.z;
            }
        }
        m_bounding_box = std::pair(low, high);
    }

}
