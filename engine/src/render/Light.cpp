#include "render/Light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "core/GameObject.hpp"
#include "render/Model.hpp"

void Light::use(Shader *lightShader, int id, int textureid) {
    genShadowMap(lightShader, id, textureid);
    std::string i = std::to_string(id);
    std::string name = (this->type == 0 ? "dirLights" : "pointLights");
    name += "[" + i + "].";
    lightShader->setInt(name + "type", type);
    lightShader->setVec3(name + "position", this->object->absPos());
    lightShader->setVec3(name + "direction", direction);
    lightShader->setVec3(name + "ambient", ambient);
    lightShader->setVec3(name + "diffuse", diffuse);
    lightShader->setFloat(name + "linear", linear);
    lightShader->setFloat(name + "quadratic", quadratic);
    lightShader->setFloat(name + "innerCutOff", innerCutOff);
    lightShader->setFloat(name + "outerCutOff", outerCutOff);
}
