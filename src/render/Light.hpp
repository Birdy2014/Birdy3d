#ifndef BIRDY3D_LIGHT_HPP
#define BIRDY3D_LIGHT_HPP

#include "Shader.hpp"

class Light {
public:
    Light(int type, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float linear, float quadratic, float innerCutOff, float outerCutOff) {
        this->type = type;
        this->position = position;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->linear = linear;
        this->quadratic = quadratic;
        this->innerCutOff = innerCutOff;
        this->outerCutOff = outerCutOff;
    }

    Light() {
        this->type = 0;
        this->position = glm::vec3(0.0f);
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
        shader.setVec3("lights[" + i + "].position", position);
        shader.setVec3("lights[" + i + "].direction", direction);
        shader.setVec3("lights[" + i + "].ambient", ambient);
        shader.setVec3("lights[" + i + "].diffuse", diffuse);
        shader.setVec3("lights[" + i + "].specular", specular);
        shader.setFloat("lights[" + i + "].linear", linear);
        shader.setFloat("lights[" + i + "].quadratic", quadratic);
        shader.setFloat("lights[" + i + "].innerCutOff", innerCutOff);
        shader.setFloat("lights[" + i + "].outerCutOff", outerCutOff);
    }

protected:
    int type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
};

#endif