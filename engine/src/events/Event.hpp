#pragma once

#include <any>

namespace Birdy3d {

    class Event {
    public:
        virtual bool checkOptions(std::any) {
            return true;
        }

        virtual ~Event() { }
    };

}
