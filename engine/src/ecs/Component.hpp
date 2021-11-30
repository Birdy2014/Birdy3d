#pragma once

#include "core/Base.hpp"
#include "ecs/Forward.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::ecs {

    class Component {
    public:
        Entity* entity = nullptr;

        virtual ~Component() = default;
        void external_start();
        void external_update();
        void external_cleanup();
        bool loaded() const { return m_loaded; }
        void remove();
        virtual void serialize(serializer::Adapter&) { }

    protected:
        bool m_loaded = false;

        virtual void start() {};
        virtual void update() {};
        virtual void cleanup() {};

        BIRDY3D_REGISTER_TYPE_DEC(Component);
    };

}
