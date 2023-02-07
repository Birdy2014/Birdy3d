#pragma once

namespace Birdy3d::events {

    class CollisionEvent;
    class Event;
    class EventBus;
    class InputClickEvent;
    class InputCharEvent;
    class InputKeyEvent;
    class InputScrollEvent;
    class ResourceLoadEvent;
    class TransformChangedEvent;
    class WindowResizeEvent;
    template <class T, class EventType>
    class MemberFunctionHandler;
    template <class EventType>
    class FunctionHandler;

}
