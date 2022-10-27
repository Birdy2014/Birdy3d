#pragma once

#include "events/Event.hpp"
#include "events/InputEvents.hpp"
#include <any>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <typeindex>
#include <utility>

namespace Birdy3d::events {

    class EventBus;

    class HandlerFunctionBase {
    public:
        HandlerFunctionBase(std::any options)
            : m_options(options)
        { }

        virtual ~HandlerFunctionBase(){};

        virtual void exec(Event* event) = 0;

    protected:
        std::any m_options;
    };

    template <class T, class EventType>
    class MemberFunctionHandler : public HandlerFunctionBase {
    public:
        typedef void (T::*MemberFunction)(EventType const&);

        MemberFunctionHandler(T* instance, MemberFunction member_function, std::any options)
            : HandlerFunctionBase(options)
            , m_instance{instance}
            , m_member_function{member_function} {};

        void exec(Event* event) override
        {
            if (m_options.has_value() && !event->check_options(m_options))
                return;
            EventType* casted_event = static_cast<EventType*>(event);
            (m_instance->*m_member_function)(*std::as_const(casted_event));
        }

    private:
        friend EventBus;
        T* m_instance;
        MemberFunction m_member_function;
    };

    template <class EventType>
    class FunctionHandler : public HandlerFunctionBase {
    public:
        typedef std::function<void(EventType const&)> HandlerFunction;

        FunctionHandler(HandlerFunction func, std::any options)
            : HandlerFunctionBase(options)
            , m_function(func)
        { }

        void exec(Event* event) override
        {
            if (m_options.has_value() && !event->check_options(m_options))
                return;
            m_function(std::as_const(*static_cast<EventType*>(event)));
        }

    private:
        friend EventBus;
        HandlerFunction m_function;
    };

    typedef std::list<std::unique_ptr<HandlerFunctionBase>> HandlerList;

    class EventBus {
    public:
        template <typename EventType, typename... Args>
        void emit(Args... args)
        {
            m_event_queue.push(std::make_unique<EventType>(args...));
        }

        void flush(int amount = -1)
        {
            for (int i = 0; (amount <= -1 || i < amount) && !m_event_queue.empty(); i++) {
                exec_first();
            }
        }

        template <class T, class EventType>
        void subscribe(T* instance, void (T::*member_function)(EventType const&), std::any options = {})
        {
            HandlerList* handlers = m_subscribers[typeid(EventType)].get();

            if (handlers == nullptr) {
                std::unique_ptr<HandlerList> handler_list = std::make_unique<HandlerList>();
                handlers = handler_list.get();
                m_subscribers[typeid(EventType)] = std::move(handler_list);
            }

            handlers->push_back(std::make_unique<MemberFunctionHandler<T, EventType>>(instance, member_function, options));
        }

        template <class EventType>
        void subscribe(std::function<void(EventType const&)> func, std::any options = {})
        {
            HandlerList* handlers = m_subscribers[typeid(EventType)].get();

            if (handlers == nullptr) {
                std::unique_ptr<HandlerList> handler_list = std::make_unique<HandlerList>();
                handlers = handler_list.get();
                m_subscribers[typeid(EventType)] = std::move(handler_list);
            }

            handlers->push_back(std::make_unique<FunctionHandler<EventType>>(func, options));
        }

        template <class T, class EventType>
        void unsubscribe(T* instance, void (T::*member_function)(EventType const&), std::any options = {})
        {
            HandlerList* handlers = m_subscribers[typeid(EventType)].get();

            if (handlers == nullptr)
                return;

            handlers->remove_if([&](std::unique_ptr<HandlerFunctionBase>& handler) {
                auto casted = dynamic_cast<MemberFunctionHandler<T, EventType>*>(handler.get());
                if (!casted)
                    return false;
                if (casted->m_instance != instance || casted->m_member_function != member_function)
                    return false;
                if (options.has_value() && !any_equals(casted->m_options, options))
                    return false;
                return true;
            });
        }

        template <class EventType>
        void unsubscribe(std::function<void(EventType const&)> func, std::any options = {})
        {
            HandlerList* handlers = m_subscribers[typeid(EventType)].get();

            if (handlers == nullptr)
                return;

            handlers->remove_if([&](std::unique_ptr<HandlerFunctionBase>& handler) {
                FunctionHandler<EventType>* casted = dynamic_cast<FunctionHandler<EventType>*>(handler.get());
                if (!casted)
                    return false;
                if (casted->m_function != func)
                    return false;
                if (options.has_value() && !any_equals(casted->m_options, options))
                    return false;
                return true;
            });
        }

    private:
        std::map<std::type_index, std::unique_ptr<HandlerList>> m_subscribers;
        std::queue<std::unique_ptr<Event>> m_event_queue;

        void exec_first()
        {
            if (m_event_queue.empty())
                return;
            Event* event = m_event_queue.front().get();
            HandlerList* handlers = m_subscribers[typeid(*event)].get();

            if (handlers == nullptr) {
                return;
            }

            for (auto& handler : *handlers) {
                if (handler != nullptr) {
                    handler->exec(event);
                }
            }
            m_event_queue.pop();
        }

        bool any_equals(std::any a, std::any b)
        {
            if (a.type() != b.type())
                return false;

            if (a.type() == typeid(int) && std::any_cast<int>(a) != std::any_cast<int>(b))
                return false;
            if (a.type() == typeid(unsigned int) && std::any_cast<unsigned int>(a) != std::any_cast<unsigned int>(b))
                return false;
            if (a.type() == typeid(long) && std::any_cast<long>(a) != std::any_cast<long>(b))
                return false;
            if (a.type() == typeid(unsigned long) && std::any_cast<unsigned long>(a) != std::any_cast<unsigned long>(b))
                return false;
            if (a.type() == typeid(std::string) && std::any_cast<std::string>(a) != std::any_cast<std::string>(b))
                return false;

            return true;
        }
    };

}
