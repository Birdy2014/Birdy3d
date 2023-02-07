#pragma once

#include "events/Event.hpp"
#include "utils/Identifier.hpp"

namespace Birdy3d::events {

    class ResourceLoadEvent : public Event {
    public:
        utils::Identifier handle_id;

        ResourceLoadEvent(utils::Identifier handle_id)
            : handle_id(handle_id)
        { }

        bool check_options(std::any options) override
        {
            if (!options.has_value())
                return true;

            return options.type() == typeid(utils::Identifier) && std::any_cast<utils::Identifier>(options) == handle_id;
        }
    };

}
