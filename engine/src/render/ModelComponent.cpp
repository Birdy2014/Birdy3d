#include "render/ModelComponent.hpp"

namespace Birdy3d::render {

    ModelComponent::ModelComponent()
        : material(nullptr) { }

    ModelComponent::ModelComponent(const std::string& name, std::shared_ptr<Material> material)
        : material(material)
        , m_model(name) { }

    void ModelComponent::start() {
        if (!m_model)
            core::Logger::warn("No model specified");
    }

    void ModelComponent::serialize(serializer::Adapter& adapter) {
        adapter("model", m_model);
        adapter("material", material);
    }

    void ModelComponent::render(const Shader& shader, bool transparent) const {
        if (m_model)
            m_model->render(*entity, material.get(), shader, transparent);
    }

    void ModelComponent::render_depth(const Shader& shader) const {
        if (m_model)
            m_model->render_depth(*entity, shader);
    }

    core::ResourceHandle<Model> ModelComponent::model() {
        return m_model;
    }

    void ModelComponent::model(const std::string& name) {
        m_model.load(name);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, ModelComponent);

}
