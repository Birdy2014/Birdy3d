#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Entity;

    class Component {
    public:
        Entity* entity = nullptr;

        virtual ~Component() = default;
        void external_start();
        void external_update();
        void external_cleanup();
        bool loaded() const { return m_loaded; }
        void remove();
        template <class Archive>
        void serialize(Archive& ar) { }

    protected:
        bool m_loaded = false;

        virtual void start() {};
        virtual void update() {};
        virtual void cleanup() {};
    };

}
