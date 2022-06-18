#pragma once

#include "core/Base.hpp"
#include "ecs/Forward.hpp"
#include "render/Material.hpp"
#include "render/Mesh.hpp"

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Birdy3d::render {

    class Model {
    public:
        Model(const std::string& path);
        Model(Mesh);
        Model(std::vector<Mesh>&);
        void render(ecs::Entity& entity, const Material* material, const Shader& shader, bool transparent) const;
        void render_depth(ecs::Entity&, const Shader&) const;
        void render_wireframe(ecs::Entity&, const Shader&) const;
        const std::vector<Mesh>& get_meshes() const;
        std::pair<glm::vec3, glm::vec3> bounding_box() const { return m_bounding_box; }

    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        Material m_embedded_material;
        std::pair<glm::vec3, glm::vec3> m_bounding_box;

        void load(std::string path);
        void process_node(aiNode* node, const aiScene* scene);
        Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
        void compute_bounding_box();
    };

}
