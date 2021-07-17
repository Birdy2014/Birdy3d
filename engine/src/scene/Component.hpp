#pragma once

namespace Birdy3d {

    class GameObject;

    class Component {
    public:
        GameObject* object = nullptr;

        void _start();
        void _update();
        void _cleanup();
        bool isLoaded();

    protected:
        bool loaded = false;

        virtual void start() {};
        virtual void update() {};
        virtual void cleanup() {};
    };

}
