#include "render/Light.hpp"

namespace Birdy3d {

    Light::Light(Shader* depthShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, float innerCutOff, float outerCutOff)
        : Component() {
        this->depthShader = depthShader;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->linear = linear;
        this->quadratic = quadratic;
        this->innerCutOff = innerCutOff;
        this->outerCutOff = outerCutOff;
    }

    Light::Light()
        : Component() {
        this->depthShader = nullptr;
        this->ambient = glm::vec3(1.0f);
        this->diffuse = glm::vec3(1.0f);
        this->linear = 1;
        this->quadratic = 1;
        this->innerCutOff = glm::cos(glm::radians(30.0f));
        this->outerCutOff = glm::cos(glm::radians(40.0f));
    }

    void Light::start() {
        setupShadowMap();
    }

    void Light::update() {
        shadowMapUpdated = false;
    }

}
