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
        Model(std::string const& path);
        Model(Mesh);
        Model(std::vector<Mesh>&);
        void render(ecs::Entity& entity, Material const* material, Shader const& shader, bool transparent) const;
        void render_depth(ecs::Entity&, Shader const&) const;
        void render_wireframe(ecs::Entity const&, Shader const&) const;
        [[nodiscard]] std::vector<Mesh> const& get_meshes() const;
        [[nodiscard]] std::pair<glm::vec3, glm::vec3> bounding_box() const { return m_bounding_box; }

    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        Material m_embedded_material;
        std::pair<glm::vec3, glm::vec3> m_bounding_box;

        void load(std::string path);
        void process_node(aiNode* node, aiScene const* scene, glm::mat4 parent_transform);
        Mesh process_mesh(aiMesh* mesh, aiScene const* scene, glm::mat4 transform);
        void compute_bounding_box();
    };

}
