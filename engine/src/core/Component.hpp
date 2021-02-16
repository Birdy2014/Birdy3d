#pragma once

namespace Birdy3d {

    class GameObject;

    class Component {
    public:
        GameObject* object = nullptr;

        Component() {};
        virtual void start() {};
        virtual void update(float deltaTime) {};
        virtual void cleanup() {};
    };

}
