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
        void render(GameObject* object, const Material* material, Shader* shader, bool transparent);
        void renderDepth(GameObject*, Shader*);
        void renderOutline(GameObject*, Shader*);
        const std::vector<Mesh*>& getMeshes();

    private:
        std::string m_path;
        std::vector<Mesh*> m_meshes;
        std::string m_directory;
        Material m_embedded_material;
        unsigned int m_outline_vao = 0;
        unsigned int m_outline_vbo = 0;

        void load();
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
    };

}
