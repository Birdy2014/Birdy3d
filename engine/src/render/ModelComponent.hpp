#pragma once

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"
#include "scene/Component.hpp"
#include <string>

namespace Birdy3d {

    class ModelComponent : public Component {
    public:
        std::shared_ptr<Model> model;
        std::string name;
        Material* material;

        ModelComponent(const std::string& name, Material* material)
            : name(name)
            , material(material) { }

        void start() override {
            model = RessourceManager::getModel(name);
        }

        void render(const Shader& shader, bool transparent) const {
            if (model)
                model->render(*object, material, shader, transparent);
            else
                Logger::error("No model specified");
        }

        void renderDepth(const Shader& shader) const {
            if (model)
                model->renderDepth(*object, shader);
        }
    };

}
