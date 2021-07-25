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
        Model(std::unique_ptr<Mesh>);
        void render(GameObject& object, const Material* material, const Shader& shader, bool transparent) const;
        void renderDepth(GameObject&, const Shader&) const;
        void render_wireframe(GameObject&, const Shader&) const;
        const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;
        std::pair<glm::vec3, glm::vec3> bounding_box() const { return m_bounding_box; }

    private:
        std::vector<std::unique_ptr<Mesh>> m_meshes;
        std::string m_directory;
        Material m_embedded_material;
        std::pair<glm::vec3, glm::vec3> m_bounding_box;

        void load(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
        void compute_bounding_box();
    };

}
