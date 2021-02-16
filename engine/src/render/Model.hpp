#pragma once

#include "core/Component.hpp"
#include "render/Mesh.hpp"
#include <assimp/scene.h>
#include <string>
#include <vector>

namespace Birdy3d {

    class Texture;

    class Model : public Component {
    public:
        Model(std::string path, bool useTexture = true, glm::vec4 color = glm::vec4(0.0f), float specular = 1, glm::vec3 emissive = glm::vec3(0.0f));
        void cleanup() override;
        void start() override;
        void render(Shader* shader, bool transparent);
        void renderDepth(Shader* shader);
        const std::vector<Mesh>& getMeshes();

    private:
        std::string path;
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;
        bool useTexture;
        glm::vec4 color;
        float specular;
        glm::vec3 emissive;

        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };

}
