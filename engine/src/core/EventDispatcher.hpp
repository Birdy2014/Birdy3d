#pragma once

#include <functional>
#include <vector>

namespace Birdy3d {

    template <class T>
    class EventDispatcher {
    private:
        struct Event {
            unsigned int type;
            std::function<void(T)> callback;
        };

    public:
        void addHandler(unsigned int type, std::function<void(T)> callback) {
            this->events.push_back(Event { type, callback });
        };

        bool removeHandler(unsigned int type, std::function<void(T)> callback) {
            events.remove(events.begin(), events.end(), Event { type, callback });
        };

        void emitEvent(unsigned int type, T arg) {
            for (Event event : this->events)
                if (event.type == type)
                    event.callback(arg);
        };

    private:
        std::vector<Event> events;
    };

}
