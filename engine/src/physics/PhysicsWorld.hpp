#pragma once

#include <memory>
#include <vector>

namespace Birdy3d {

    class GameObject;
    class Collision;

    class PhysicsWorld {
    public:
        PhysicsWorld(GameObject* scene);
        ~PhysicsWorld();
        void update();

    private:
        GameObject* m_scene;
        std::vector<std::unique_ptr<Collision>> m_collisions;
    };

}
