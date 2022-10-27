#include "render/ModelComponent.hpp"

namespace Birdy3d::render {

    ModelComponent::ModelComponent()
        : material(nullptr)
    { }

    ModelComponent::ModelComponent(std::string const& name, std::shared_ptr<Material> material)
        : material(material)
        , m_model(name)
    { }

    void ModelComponent::start()
    {
        if (!m_model)
            core::Logger::warn("No model specified");
    }

    void ModelComponent::serialize(serializer::Adapter& adapter)
    {
        adapter("model", m_model);
        adapter("material", material);
    }

    void ModelComponent::render(Shader const& shader, bool transparent) const
    {
        if (m_model)
            m_model->render(*entity, material.get(), shader, transparent);
    }

    void ModelComponent::render_depth(Shader const& shader) const
    {
        if (m_model)
            m_model->render_depth(*entity, shader);
    }

    core::ResourceHandle<Model> ModelComponent::model()
    {
        return m_model;
    }

    void ModelComponent::model(std::string const& name)
    {
        m_model = name;
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, ModelComponent);

}
