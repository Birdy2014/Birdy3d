#pragma once

#include "render/Mesh.hpp"
#include "render/ModelOptions.hpp"
#include <assimp/scene.h>
#include <string>
#include <vector>

namespace Birdy3d {

    class GameObject;
    class Texture;

    class Model {
    public:
        Model(const std::string& path);
        ~Model();
        void render(GameObject* object, ModelOptions options, Shader* shader, bool transparent);
        void renderDepth(GameObject* object, Shader* shader);
        const std::vector<Mesh*>& getMeshes();

    private:
        std::string path;
        std::vector<Mesh*> meshes;
        std::string directory;
        std::vector<Texture*> textures_loaded;

        void load();
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };

}
