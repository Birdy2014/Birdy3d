#include "render/Light.hpp"

namespace Birdy3d {

    Light::Light(bool shadow_enabled)
        : Component()
        , shadow_enabled(shadow_enabled) { }

    void Light::start() {
        setup_shadow_map();
    }

    void Light::update() {
        if (shadow_enabled)
            m_shadowMapUpdated = false;
    }

    void Light::cleanup() { }

}
