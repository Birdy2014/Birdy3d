#pragma once

#include "core/Component.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"
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

        void render(Shader* shader, bool transparent) {
            if (model)
                model->render(object, material, shader, transparent);
            else
                Logger::error("No model specified");
        }

        void renderDepth(Shader* shader) {
            if (model)
                model->renderDepth(object, shader);
        }
    };

}
