#pragma once

#include "core/Component.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/Model.hpp"
#include "render/ModelOptions.hpp"
#include <string>

namespace Birdy3d {

    class ModelComponent : public Component {
    public:
        Model* model;
        std::string name;
        ModelOptions options;

        ModelComponent(const std::string& name, bool useTexture = true, glm::vec4 color = glm::vec4(0), float specular = 1, glm::vec3 emissive = glm::vec3(0))
            : name(name) {
            this->options = ModelOptions(useTexture, color, specular, emissive);
        }

        void start() override {
            this->model = RessourceManager::getModel(name);
        }

        void render(Shader* shader, bool transparent) {
            if (this->model)
                this->model->render(this->object, options, shader, transparent);
            else
                Logger::error("No model specified");
        }

        void renderDepth(Shader* shader) {
            if (this->model)
                this->model->renderDepth(this->object, shader);
        }
    };

}
