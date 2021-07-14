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

    void Model::renderOutline(GameObject* object, Shader* shader) {
        if (m_outline_vao == 0) {
            const float outline_offset = 0.4;

            float x_low = std::numeric_limits<float>::max();
            float y_low = std::numeric_limits<float>::max();
            float z_low = std::numeric_limits<float>::max();
            float x_high = std::numeric_limits<float>::min();
            float y_high = std::numeric_limits<float>::min();
            float z_high = std::numeric_limits<float>::min();

            for (Mesh* mesh : m_meshes) {
                for (Vertex vertex : mesh->vertices) {
                    if (vertex.position.x < x_low)
                        x_low = vertex.position.x;
                    if (vertex.position.y < y_low)
                        y_low = vertex.position.y;
                    if (vertex.position.z < z_low)
                        z_low = vertex.position.z;
                    if (vertex.position.x > x_high)
                        x_high = vertex.position.x;
                    if (vertex.position.y > y_high)
                        y_high = vertex.position.y;
                    if (vertex.position.z > z_high)
                        z_high = vertex.position.z;
                }
            }

            x_low -= outline_offset;
            y_low -= outline_offset;
            z_low -= outline_offset;
            x_high += outline_offset;
            y_high += outline_offset;
            z_high += outline_offset;

            // clang-format off
            glm::vec3 vertices[24] = {
                // Bottom rectangle
                glm::vec3(x_low, y_low, z_low), glm::vec3(x_high, y_low, z_low),
                glm::vec3(x_high, y_low, z_low), glm::vec3(x_high, y_low, z_high),
                glm::vec3(x_high, y_low, z_high), glm::vec3(x_low, y_low, z_high),
                glm::vec3(x_low, y_low, z_high), glm::vec3(x_low, y_low, z_low),
                // Top rectangle
                glm::vec3(x_low, y_high, z_low), glm::vec3(x_high, y_high, z_low),
                glm::vec3(x_high, y_high, z_low), glm::vec3(x_high, y_high, z_high),
                glm::vec3(x_high, y_high, z_high), glm::vec3(x_low, y_high, z_high),
                glm::vec3(x_low, y_high, z_high), glm::vec3(x_low, y_high, z_low),
                // Side rectangles
                glm::vec3(x_low, y_low, z_low), glm::vec3(x_low, y_high, z_low),
                glm::vec3(x_high, y_low, z_low), glm::vec3(x_high, y_high, z_low),
                glm::vec3(x_high, y_low, z_high), glm::vec3(x_high, y_high, z_high),
                glm::vec3(x_low, y_low, z_high), glm::vec3(x_low, y_high, z_high),
            };
            // clang-format on

            glGenVertexArrays(1, &m_outline_vao);
            glGenBuffers(1, &m_outline_vbo);

            glBindVertexArray(m_outline_vao);

            glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);
            glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }
        glm::mat4 model = object->transform.matrix();
        shader->use();
        shader->setMat4("model", model);
        glBindVertexArray(m_outline_vao);
        glDrawArrays(GL_LINES, 0, 24);
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

}
