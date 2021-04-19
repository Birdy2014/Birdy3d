#pragma once

#include "core/Component.hpp"
#include "core/GameObject.hpp"
#include "events/Event.hpp"
#include <functional>
#include <list>
#include <map>
#include <typeindex>

namespace Birdy3d {

    class EventBus;

    class HandlerFunctionBase {
    public:
        virtual void exec(Event* event) = 0;
    };

    template<class T, class EventType>
    class MemberFunctionHandler : public HandlerFunctionBase {
    public:
        typedef void (T::*MemberFunction)(EventType*);

        MemberFunctionHandler(T* instance, MemberFunction memberFunction)
            : instance { instance }
            , memberFunction { memberFunction } {};

        void exec(Event* event) override {
            Component* component = dynamic_cast<Component*>(instance);
            EventType* castedEvent = static_cast<EventType*>(event);
            if (component && !castedEvent->forObject(component->object))
                return;
            (instance->*memberFunction)(castedEvent);
        }

    private:
        friend EventBus;
        T* instance;
        MemberFunction memberFunction;
    };

    template<class EventType>
    class FunctionHandler : public HandlerFunctionBase {
    public:
        typedef std::function<void(EventType*)> HandlerFunction;

        FunctionHandler(HandlerFunction func, GameObject* target = nullptr)
            : function(func)
            , target(target) { }

        void exec(Event* event) override {
            EventType* castedEvent = static_cast<EventType*>(event);
            if (target && !castedEvent->forObject(target))
                return;
            function(static_cast<EventType*>(event));
        }

    private:
        friend EventBus;
        GameObject* target;
        HandlerFunction function;
    };

    typedef std::list<HandlerFunctionBase*> HandlerList;

    class EventBus {
    public:
        template<typename EventType>
        void emit(EventType* event) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                return;
            }

            for (auto& handler : *handlers) {
                if (handler != nullptr) {
                    handler->exec(event);
                }
            }
            delete event;
        }

        template<class T, class EventType>
        void subscribe(T* instance, void (T::*memberFunction)(EventType*)) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }

            handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
        }

        template<class EventType>
        void subscribe(std::function<void(EventType*)> func, GameObject* target = nullptr) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }

            handlers->push_back(new FunctionHandler<EventType>(func, target));
        }

        template<class T, class EventType>
        void unsubscribe(T* instance, void (T::*memberFunction)(EventType*)) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr)
                return;

            for (HandlerFunctionBase* handler : *handlers) {
                MemberFunctionHandler<T, EventType>* casted = dynamic_cast<MemberFunctionHandler<T, EventType>*>(handler);
                if (!casted)
                    continue;
                if (casted->instance != instance || casted->memberFunction != memberFunction)
                    continue;
                handlers->remove(handler);
                delete handler;
                return;
            }
        }

        template<class EventType>
        void unsubscribe(std::function<void(EventType*)> func, GameObject* target = nullptr) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr)
                return;

            for (HandlerFunctionBase* handler : *handlers) {
                FunctionHandler<EventType>* casted = dynamic_cast<FunctionHandler<EventType>*>(handler);
                if (!casted)
                    continue;
                if (casted->function != func || casted->target != target)
                    continue;
                handlers->remove(handler);
                delete handler;
                return;
            }
        }

    private:
        std::map<std::type_index, HandlerList*> subscribers;
    };

}
