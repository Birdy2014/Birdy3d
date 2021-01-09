#include "render/Light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "core/GameObject.hpp"
#include "render/Model.hpp"

void Light::start() {
    setupShadowMap();
}

void Light::update(float deltaTime) {
    shadowMapUpdated = false;
}
