#ifndef BIRDY3D_EVENTHANDLER_HPP
#define BIRDY3D_EVENTHANDLER_HPP

#include <functional>
#include <vector>

template<class T>
class EventHandler {
public:
    void addHandler(std::function<void(T)> callback) {
        this->callbacks.push_back(callback);
    };

    bool removeHandler(std::function<void(T)> callback) {
        callbacks.remove(callbacks.begin(), callbacks.end(), callback);
    };

    void emitEvent(T arg) {
        for (std::function<void(T)> callback : this->callbacks)
            callback(arg);
    };

private:
    std::vector<std::function<void(T)>> callbacks;
};

#endif
