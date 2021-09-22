#pragma once

#include <memory>
#include <vector>

namespace Birdy3d {

    class Entity;
    class Collision;

    class PhysicsWorld {
    public:
        PhysicsWorld(Entity* scene);
        ~PhysicsWorld();
        void update();

    private:
        Entity* m_scene;
        std::vector<std::unique_ptr<Collision>> m_collisions;
    };

}
