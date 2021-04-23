#pragma once

namespace Birdy3d {

    class GameObject;

    class Event {
    public:
        virtual bool forObject(GameObject* obj) {
            return true;
        }

        virtual bool checkOptions(int options) {
            return true;
        }

    protected:
        virtual ~Event() { }
    };

}
