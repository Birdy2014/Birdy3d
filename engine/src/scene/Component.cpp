#include "scene/Component.hpp"

#include "scene/GameObject.hpp"

namespace Birdy3d {

    void Component::_start() {
        if (!loaded) {
            start();
            loaded = true;
        }
    }

    void Component::_update() {
        if (loaded)
            update();
        else
            _start();
    }

    void Component::_cleanup() {
        if (loaded)
            cleanup();
    }

    bool Component::isLoaded() {
        return loaded;
    }

    void Component::remove() {
        if (object)
            object->remove_component(this);
    }

}
