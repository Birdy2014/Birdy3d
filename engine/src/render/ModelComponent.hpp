#pragma once

#include "core/Base.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Component.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"

namespace Birdy3d::render {

    class ModelComponent : public ecs::Component {
    public:
        std::shared_ptr<Material> material;

        ModelComponent();
        ModelComponent(const std::string& name, std::shared_ptr<Material> material = {});
        void start() override;
        void serialize(serializer::Adapter& adapter) override;
        void render(const Shader& shader, bool transparent) const;
        void render_depth(const Shader& shader) const;
        core::ResourceHandle<Model> model();
        void model(const std::string& name);

    private:
        core::ResourceHandle<Model> m_model;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, ModelComponent);
    };

}
