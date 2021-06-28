#pragma once

#include "core/GameObject.hpp"

namespace Birdy3d {

    class Camera;

    class Scene : public GameObject {
    public:
        Camera* m_current_camera;

        void start() override {
            setScene(this);
            GameObject::start();
        }
    };

}
