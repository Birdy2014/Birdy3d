#include "scene/Component.hpp"

#include "scene/GameObject.hpp"

namespace Birdy3d {

    void Component::_start() {
        if (!m_loaded) {
            start();
            m_loaded = true;
        }
    }

    void Component::_update() {
        if (m_loaded)
            update();
        else
            _start();
    }

    void Component::_cleanup() {
        if (m_loaded)
            cleanup();
    }

    void Component::remove() {
        if (object)
            object->remove_component(this);
    }

}
