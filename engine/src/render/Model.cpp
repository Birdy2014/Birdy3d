#include "render/Model.hpp"

#include "core/GameObject.hpp"
#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Birdy3d {

    Model::Model(const std::string& path)
        : path(path) {
        Logger::debug("Loading model: " + path);
        load();
    }

    void Model::render(GameObject* object, ModelOptions options, Shader* shader, bool transparent) {
        glm::mat4 model = object->transform.matrix();
        shader->use();
        shader->setMat4("model", model);
        for (Mesh& m : this->meshes) {
            if (transparent == m.hasTransparency(options))
                m.render(shader, options);
        }
    }

    void Model::renderDepth(GameObject* object, Shader* shader) {
        glm::mat4 model = object->transform.matrix();
        shader->use();
        shader->setMat4("model", model);
        for (Mesh m : meshes) {
            m.renderDepth();
        }
    }

    const std::vector<Mesh>& Model::getMeshes() {
        return meshes;
    }

    void Model::load() {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::error(std::string("ASSIMP: ") + importer.GetErrorString());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        // process own meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

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
        if (options.useTexture) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

            return Mesh(vertices, indices, textures);
        } else {
            return Mesh(vertices, indices);
        }
    }

    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (textures_loaded[j].path == std::string(str.C_Str())) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture(directory + "/" + std::string(str.C_Str()), typeName, std::string(str.C_Str()));
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    Model::~Model() {
        for (Mesh& m : this->meshes) {
            m.cleanup();
        }
    }

}
