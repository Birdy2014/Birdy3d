#pragma once

#include <any>

namespace Birdy3d {

    class Event {
    public:
        virtual bool check_options(std::any) {
            return true;
        }

        virtual ~Event() { }
    };

}
