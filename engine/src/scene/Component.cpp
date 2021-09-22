#include "scene/Component.hpp"

#include "scene/Entity.hpp"

namespace Birdy3d {

    void Component::external_start() {
        if (!m_loaded) {
            start();
            m_loaded = true;
        }
    }

    void Component::external_update() {
        if (m_loaded)
            update();
        else
            external_start();
    }

    void Component::external_cleanup() {
        if (m_loaded)
            cleanup();
    }

    void Component::remove() {
        if (entity)
            entity->remove_component(this);
    }

}
