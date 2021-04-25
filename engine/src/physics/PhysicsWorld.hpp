#pragma once

#include <vector>

namespace Birdy3d {

    class GameObject;
    class Collision;

    class PhysicsWorld {
    public:
        PhysicsWorld(GameObject* scene);
        void update();

    private:
        GameObject* scene;
        std::vector<Collision*> collisions;
    };

}
