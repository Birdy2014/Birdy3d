#pragma once

#include "core/Base.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Component.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"

namespace Birdy3d::render {

    class ModelComponent : public ecs::Component {
    public:
        ModelComponent();
        ModelComponent(core::ResourceHandle<Model> model, std::shared_ptr<Material> = {});
        ModelComponent(std::string model, std::shared_ptr<Material> = {});
        void start() override;
        void serialize(serializer::Adapter& adapter) override;
        void render(const Shader& shader, bool transparent) const;
        void render_depth(const Shader& shader) const;

        core::ResourceHandle<Model> model();
        void model(core::ResourceHandle<Model>);
        const Material* material();
        void material(std::shared_ptr<Material>);

    private:
        core::ResourceHandle<Model> m_model;
        std::shared_ptr<Material> m_material;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, ModelComponent);
    };

}
