#pragma once

#include "core/Component.hpp"
#include "core/GameObject.hpp"
#include "events/Event.hpp"
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <typeindex>

namespace Birdy3d {

    class EventBus;

    class HandlerFunctionBase {
    public:
        HandlerFunctionBase(int options)
            : options(options) { }

        virtual ~HandlerFunctionBase() {};

        virtual void exec(Event* event) = 0;

    protected:
        int options;
    };

    template <class T, class EventType>
    class MemberFunctionHandler : public HandlerFunctionBase {
    public:
        typedef void (T::*MemberFunction)(EventType*);

        MemberFunctionHandler(T* instance, MemberFunction memberFunction, int options)
            : HandlerFunctionBase(options)
            , instance { instance }
            , memberFunction { memberFunction } {};

        void exec(Event* event) override {
            if (options != -1 && !event->checkOptions(options))
                return;
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

    template <class EventType>
    class FunctionHandler : public HandlerFunctionBase {
    public:
        typedef std::function<void(EventType*)> HandlerFunction;

        FunctionHandler(HandlerFunction func, GameObject* target, int options)
            : HandlerFunctionBase(options)
            , function(func)
            , target(target) { }

        void exec(Event* event) override {
            if (options != -1 && !event->checkOptions(options))
                return;
            EventType* castedEvent = static_cast<EventType*>(event);
            if (target && !castedEvent->forObject(target))
                return;
            function(static_cast<EventType*>(event));
        }

    private:
        friend EventBus;
        HandlerFunction function;
        GameObject* target;
    };

    typedef std::list<HandlerFunctionBase*> HandlerList;

    class EventBus {
    public:
        template <typename EventType, typename... Args>
        void emit(Args... args) {
            eventQueue.push(std::make_unique<EventType>(args...));
        }

        void flush(int amount = -1) {
            for (int i = 0; (amount <= -1 || i < amount) && !eventQueue.empty(); i++) {
                execFirst();
            }
        }

        template <class T, class EventType>
        void subscribe(T* instance, void (T::*memberFunction)(EventType*), int options = -1) {
            HandlerList* handlers = subscribers[typeid(EventType)].get();

            if (handlers == nullptr) {
                std::unique_ptr<HandlerList> handler_list = std::make_unique<HandlerList>();
                handlers = handler_list.get();
                subscribers[typeid(EventType)] = std::move(handler_list);
            }

            handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction, options));
        }

        template <class EventType>
        void subscribe(std::function<void(EventType*)> func, GameObject* target = nullptr, int options = -1) {
            HandlerList* handlers = subscribers[typeid(EventType)].get();

            if (handlers == nullptr) {
                std::unique_ptr<HandlerList> handler_list = std::make_unique<HandlerList>();
                handlers = handler_list.get();
                subscribers[typeid(EventType)] = std::move(handler_list);
            }

            handlers->push_back(new FunctionHandler<EventType>(func, target, options));
        }

        template <class EventType>
        void subscribe(std::function<void(EventType*)> func, int options) {
            subscribe(func, nullptr, options);
        }

        template <class T, class EventType>
        void unsubscribe(T* instance, void (T::*memberFunction)(EventType*), int options = -1) {
            HandlerList* handlers = subscribers[typeid(EventType)].get();

            if (handlers == nullptr)
                return;

            for (HandlerFunctionBase* handler : *handlers) {
                MemberFunctionHandler<T, EventType>* casted = dynamic_cast<MemberFunctionHandler<T, EventType>*>(handler);
                if (!casted)
                    continue;
                if (casted->instance != instance || casted->memberFunction != memberFunction)
                    continue;
                if (options != -1 && casted->options != options)
                    continue;
                handlers->remove(handler);
                delete handler;
                return;
            }
        }

        template <class EventType>
        void unsubscribe(std::function<void(EventType*)> func, GameObject* target = nullptr, int options = -1) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr)
                return;

            for (HandlerFunctionBase* handler : *handlers) {
                FunctionHandler<EventType>* casted = dynamic_cast<FunctionHandler<EventType>*>(handler);
                if (!casted)
                    continue;
                if (casted->function != func || casted->target != target)
                    continue;
                if (options != -1 && casted->options != options)
                    continue;
                handlers->remove(handler);
                delete handler;
                return;
            }
        }

        template <class EventType>
        void unsubscribe(std::function<void(EventType*)> func, int options) {
            unsubscribe(func, nullptr, options);
        }

    private:
        std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
        std::queue<std::unique_ptr<Event>> eventQueue;

        void execFirst() {
            if (eventQueue.empty())
                return;
            Event* event = eventQueue.front().get();
            HandlerList* handlers = subscribers[typeid(*event)].get();

            if (handlers == nullptr) {
                return;
            }

            for (auto& handler : *handlers) {
                if (handler != nullptr) {
                    handler->exec(event);
                }
            }
            eventQueue.pop();
        }
    };

}
