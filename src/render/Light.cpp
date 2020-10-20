#include "Light.hpp"
#include "Model.hpp"

void Light::use(int id, int textureid) {
    genShadowMap(id, textureid);
    std::string i = std::to_string(id);
    std::string name = (this->type == 0 ? "dirLights" : "pointLights");
    name += "[" + i + "].";
    this->lightShader->setInt(name + "type", type);
    this->lightShader->setVec3(name + "position", this->object->absPos());
    this->lightShader->setVec3(name + "direction", direction);
    this->lightShader->setVec3(name + "ambient", ambient);
    this->lightShader->setVec3(name + "diffuse", diffuse);
    this->lightShader->setFloat(name + "linear", linear);
    this->lightShader->setFloat(name + "quadratic", quadratic);
    this->lightShader->setFloat(name + "innerCutOff", innerCutOff);
    this->lightShader->setFloat(name + "outerCutOff", outerCutOff);
}
