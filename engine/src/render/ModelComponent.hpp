#pragma once

#include "core/Base.hpp"
#include "ecs/Component.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"

namespace Birdy3d::render {

    class ModelComponent : public ecs::Component {
    public:
        std::shared_ptr<Material> material;

        ModelComponent();
        ModelComponent(std::string const& name, std::shared_ptr<Material> material = {});
        void start() override;
        void serialize(serializer::Adapter& adapter) override;
        void render(Shader const& shader, bool transparent) const;
        void render_depth(Shader const& shader) const;
        core::ResourceHandle<Model> model();
        void model(std::string const& name);

    private:
        core::ResourceHandle<Model> m_model;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, ModelComponent);
    };

}
