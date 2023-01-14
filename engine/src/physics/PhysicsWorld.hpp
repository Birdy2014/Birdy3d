#pragma once

#include "ecs/Forward.hpp"
#include "physics/Forward.hpp"
#include <memory>
#include <vector>

namespace Birdy3d::physics {

    class PhysicsWorld {
    public:
        PhysicsWorld(ecs::Entity* scene);
        ~PhysicsWorld();
        void update();

    private:
        ecs::Entity* m_scene;
        std::vector<Collision> m_collisions;
    };

}
