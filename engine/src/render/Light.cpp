#include "render/Light.hpp"

namespace Birdy3d {

    Light::Light(std::shared_ptr<Shader> depthShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, bool shadow_enabled)
        : Component()
        , shadow_enabled(shadow_enabled)
        , ambient(ambient)
        , diffuse(diffuse)
        , linear(linear)
        , quadratic(quadratic)
        , m_depthShader(depthShader) { }

    void Light::start() {
        setupShadowMap();
    }

    void Light::update() {
        if (shadow_enabled)
            m_shadowMapUpdated = false;
    }

    void Light::cleanup() { }

}
