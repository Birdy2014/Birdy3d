#ifndef BIRDY3D_LIGHT_HPP
#define BIRDY3D_LIGHT_HPP

#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "../objects/Component.hpp"
#include "../objects/GameObject.hpp"
#include <typeinfo>

class Light : public Component {
public:
    Light(GameObject *o, int type, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float linear, float quadratic, float innerCutOff, float outerCutOff) : Component(o) {
        this->type = type;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->linear = linear;
        this->quadratic = quadratic;
        this->innerCutOff = innerCutOff;
        this->outerCutOff = outerCutOff;
    }

    Light(GameObject *o) : Component(o) {
        this->type = 0;
        this->direction = glm::vec3(1.0f);
        this->ambient = glm::vec3(1.0f);
        this->diffuse = glm::vec3(1.0f);
        this->specular = glm::vec3(1.0f);
        this->linear = 1;
        this->quadratic = 1;
        this->innerCutOff = glm::cos(glm::radians(30.0f));
        this->outerCutOff = glm::cos(glm::radians(40.0f));
    }

    void use(Shader *shader, int id);
    void setupShadowMap();
    void genShadowMap(Shader *lightShader, int id);
    void start() override {
        setupShadowMap();
    }
    void update(float deltaTime) override {}
    void cleanup() override {}
    const std::type_info &getTypeid() override {
        return typeid(Light);
    }

protected:
    int type;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
    unsigned int depthMapFBO, depthMap;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
};

#endif