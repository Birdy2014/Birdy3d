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
        Model(Mesh*);
        ~Model();
        void render(GameObject* object, const Material* material, Shader* shader, bool transparent);
        void renderDepth(GameObject*, Shader*);
        const std::vector<Mesh*>& getMeshes();
        std::pair<glm::vec3, glm::vec3> bounding_box() { return m_bounding_box; }

    private:
        std::vector<Mesh*> m_meshes;
        std::string m_directory;
        Material m_embedded_material;
        std::pair<glm::vec3, glm::vec3> m_bounding_box;

        void load(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
        void compute_bounding_box();
    };

}
