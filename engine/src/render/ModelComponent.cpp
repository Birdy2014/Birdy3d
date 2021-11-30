#include "render/ModelComponent.hpp"

namespace Birdy3d::render {

    ModelComponent::ModelComponent()
        : material(nullptr)
        , m_model_name("") { }

    ModelComponent::ModelComponent(const std::string& name, std::shared_ptr<Material> material)
        : material(material)
        , m_model_name(name) { }

    void ModelComponent::start() {
        m_model = core::ResourceManager::get_model(m_model_name);
    }

    void ModelComponent::serialize(serializer::Adapter& adapter) {
        adapter("model_name", m_model_name);
        adapter("material", material);
    }

    void ModelComponent::render(const Shader& shader, bool transparent) const {
        if (m_model)
            m_model->render(*entity, material.get(), shader, transparent);
        else
            core::Logger::error("No model specified");
    }

    void ModelComponent::render_depth(const Shader& shader) const {
        if (m_model)
            m_model->render_depth(*entity, shader);
    }

    std::shared_ptr<Model> ModelComponent::model() {
        return m_model;
    }

    void ModelComponent::model(const std::string& name) {
        m_model_name = name;
        m_model = core::ResourceManager::get_model(name);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, ModelComponent);

}
