#include "render/Model.hpp"

#include "core/GameObject.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Birdy3d {

    Model::Model(const std::string& path)
        : m_path(path) {
        Logger::debug("Loading model: ", path);
        load();
        compute_bounding_box();
    }

    void Model::render(GameObject* object, const Material* material, Shader* shader, bool transparent) {
        if (material == nullptr)
            material = &m_embedded_material;
        glm::mat4 model = object->transform.matrix();
        shader->use();
        shader->setMat4("model", model);
        for (Mesh* m : m_meshes) {
            if (transparent == material->transparent())
                m->render(shader, *material);
        }
    }

    void Model::renderDepth(GameObject* object, Shader* shader) {
        glm::mat4 model = object->transform.matrix();
        shader->use();
        shader->setMat4("model", model);
        for (Mesh* m : m_meshes) {
            m->renderDepth();
        }
    }

    const std::vector<Mesh*>& Model::getMeshes() {
        return m_meshes;
    }

    void Model::load() {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(m_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::error("ASSIMP: ", importer.GetErrorString());
            return;
        }
        m_directory = m_path.substr(0, m_path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        // process own meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        // children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene) {
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
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.texCoords = glm::vec2(0);

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
            m_embedded_material.diffuse_map = RessourceManager::getTexture(m_directory + "/" + path.C_Str());
            m_embedded_material.diffuse_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            material->GetTexture(aiTextureType_SPECULAR, 0, &path);
            m_embedded_material.specular_map = RessourceManager::getTexture(m_directory + "/" + path.C_Str());
            m_embedded_material.specular_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
            material->GetTexture(aiTextureType_NORMALS, 0, &path);
            m_embedded_material.normal_map = RessourceManager::getTexture(m_directory + "/" + path.C_Str());
            m_embedded_material.normal_map_enabled = true;
        }

        if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
            material->GetTexture(aiTextureType_EMISSIVE, 0, &path);
            m_embedded_material.emissive_map = RessourceManager::getTexture(m_directory + "/" + path.C_Str());
            m_embedded_material.emissive_map_enabled = true;
        }

        return new Mesh(vertices, indices);
    }

    Model::~Model() {
        for (Mesh* m : m_meshes) {
            delete m;
        }
    }

    void Model::compute_bounding_box() {
        glm::vec3 low(std::numeric_limits<float>::max());
        glm::vec3 high(std::numeric_limits<float>::min());
        for (Mesh* mesh : m_meshes) {
            for (Vertex vertex : mesh->vertices) {
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
