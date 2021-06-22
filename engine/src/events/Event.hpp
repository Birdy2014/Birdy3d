#pragma once

namespace Birdy3d {

    class GameObject;

    class Event {
    public:
        virtual bool forObject(GameObject* obj [[maybe_unused]]) {
            return true;
        }

        virtual bool checkOptions(int options [[maybe_unused]]) {
            return true;
        }

        virtual ~Event() { }
    };

}
