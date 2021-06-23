#pragma once

#include "render/Material.hpp"
#include "render/Mesh.hpp"
#include <string>
#include <vector>

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Birdy3d {

    class GameObject;

    class Model {
    public:
        Model(const std::string& path);
        ~Model();
        void render(GameObject* object, const Material& material, Shader* shader, bool transparent);
        void renderDepth(GameObject* object, Shader* shader);
        const std::vector<Mesh*>& getMeshes();

    private:
        std::string path;
        std::vector<Mesh*> meshes;
        std::string directory;

        void load();
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
    };

}
