#pragma once

#include "core/Base.hpp"
#include "core/RessourceManager.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"
#include "scene/Component.hpp"

namespace Birdy3d {

    class ModelComponent : public Component {
    public:
        std::shared_ptr<Material> material;

        ModelComponent()
            : material(nullptr)
            , m_model_name("") { }

        ModelComponent(const std::string& name, std::shared_ptr<Material> material = {})
            : material(material)
            , m_model_name(name) { }

        void start() override {
            m_model = RessourceManager::getModel(m_model_name);
        }

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("model_name", m_model_name));
            ar(cereal::make_nvp("material", material));
        }

        void render(const Shader& shader, bool transparent) const {
            if (m_model)
                m_model->render(*object, material.get(), shader, transparent);
            else
                Logger::error("No model specified");
        }

        void renderDepth(const Shader& shader) const {
            if (m_model)
                m_model->renderDepth(*object, shader);
        }

        std::shared_ptr<Model> model() { return m_model; }
        void model(const std::string& name) {
            m_model_name = name;
            m_model = RessourceManager::getModel(name);
        }

    private:
        std::string m_model_name;
        std::shared_ptr<Model> m_model;
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::ModelComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Component, Birdy3d::ModelComponent);
