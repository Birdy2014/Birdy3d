#ifndef BIRDY3D_LIGHT_HPP
#define BIRDY3D_LIGHT_HPP

#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "../objects/Component.hpp"
#include "../objects/GameObject.hpp"
#include <typeinfo>

class Light : public Component {
public:
    Light(GameObject *o, Shader *depthShader, Shader *lightShader, int type, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, float innerCutOff, float outerCutOff) : Component(o) {
        this->depthShader = depthShader;
        this->lightShader = lightShader;
        this->type = type;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->linear = linear;
        this->quadratic = quadratic;
        this->innerCutOff = innerCutOff;
        this->outerCutOff = outerCutOff;
    }

    Light(GameObject *o, Shader *depthShader, Shader *lightShader) : Component(o) {
        this->depthShader = depthShader;
        this->lightShader = lightShader;
        this->type = 0;
        this->direction = glm::vec3(1.0f);
        this->ambient = glm::vec3(1.0f);
        this->diffuse = glm::vec3(1.0f);
        this->linear = 1;
        this->quadratic = 1;
        this->innerCutOff = glm::cos(glm::radians(30.0f));
        this->outerCutOff = glm::cos(glm::radians(40.0f));
    }

    void use(int id, int textureid);
    virtual void setupShadowMap() {}
    virtual void genShadowMap(int id, int textureid) {}
    void start() override {
        setupShadowMap();
    }
    void update(float deltaTime) override {}
    void cleanup() override {}
    const std::type_info &getTypeid() override {
        return typeid(Light);
    }
    Shader *getLightShader() {
        return this->lightShader;
    }

protected:
    Shader *depthShader;
    Shader *lightShader;
    int type;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
    unsigned int depthMapFBO, depthMap;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
};

#endif