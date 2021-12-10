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

        /**
         * @brief Get the loading priority of the component.
         *
         * A lower value results in an earlier placement in the entity,
         * which affects the timing of start, update and cleanup.
         * The default for components which don't override this function is 0.
         * Only override this function, if the component reqires an other component to be loaded.
         *
         * @returns The priority.
         */
        virtual int priority() { return 0; }

    protected:
        bool m_loaded = false;

        virtual void start() {};
        virtual void update() {};
        virtual void cleanup() {};

        BIRDY3D_REGISTER_TYPE_DEC(Component);
    };

}
