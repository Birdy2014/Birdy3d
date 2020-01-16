#ifndef BIRDY3D_LIGHT_HPP
#define BIRDY3D_LIGHT_HPP

#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "../objects/Component.hpp"
#include "../objects/GameObject.hpp"
#include <typeinfo>

class Light : public Component {
public:
    Light(GameObject *o, int id, int type, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float linear, float quadratic, float innerCutOff, float outerCutOff) : Component(o) {
        this->id = id;
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

    Light(GameObject *o, int id) : Component(o) {
        this->id = id;
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

    void use(Shader shader, int id) {
        std::string i = std::to_string(id);
        shader.setInt("lights[" + i + "].type", type);
        shader.setVec3("lights[" + i + "].position", this->object->absPos());
        shader.setVec3("lights[" + i + "].direction", direction);
        shader.setVec3("lights[" + i + "].ambient", ambient);
        shader.setVec3("lights[" + i + "].diffuse", diffuse);
        shader.setVec3("lights[" + i + "].specular", specular);
        shader.setFloat("lights[" + i + "].linear", linear);
        shader.setFloat("lights[" + i + "].quadratic", quadratic);
        shader.setFloat("lights[" + i + "].innerCutOff", innerCutOff);
        shader.setFloat("lights[" + i + "].outerCutOff", outerCutOff);
    }

    void setupShadowMap();
    void genShadowMap();
    void start() override {
        setupShadowMap();
    }
    void update(float deltaTime) override {
        genShadowMap();
        use(*this->object->shader, id);
    }
    void cleanup() override {}
    const std::type_info &getTypeid() override {
        return typeid(Light);
    }

protected:
    int id;
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