#include "render/ModelComponent.hpp"

namespace Birdy3d::render {

    ModelComponent::ModelComponent()
        : m_material(nullptr) { }

    ModelComponent::ModelComponent(core::ResourceHandle<Model> model, std::shared_ptr<Material> material)
        : m_model(model)
        , m_material(material) { }

    ModelComponent::ModelComponent(std::string model, std::shared_ptr<Material> material)
        : m_model(model)
        , m_material(material) { }

    void ModelComponent::start() {
        if (!m_model)
            core::Logger::warn("No model specified");
    }

    void ModelComponent::serialize(serializer::Adapter& adapter) {
        adapter("model", m_model);
        adapter("material", m_material);
    }

    void ModelComponent::render(const Shader& shader, bool transparent) const {
        if (m_model)
            m_model->render(*entity, m_material.get(), shader, transparent);
    }

    void ModelComponent::render_depth(const Shader& shader) const {
        if (m_model)
            m_model->render_depth(*entity, shader);
    }

    core::ResourceHandle<Model> ModelComponent::model() {
        return m_model;
    }

    void ModelComponent::model(core::ResourceHandle<Model> model) {
        m_model = model;
    }

    const Material* ModelComponent::material() {
        if (m_material)
            return m_material.get();
        if (m_model)
            return &m_model->embedded_material();
        return {};
    }

    void ModelComponent::material(std::shared_ptr<Material> material) {
        m_material = material;
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, ModelComponent);

}
