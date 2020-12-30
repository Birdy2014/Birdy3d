#ifndef BIRDY3D_ENGINE_HPP
#define BIRDY3D_ENGINE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/EventDispatcher.hpp"

class Application {
public:
    enum EventTypes {
        EVENT_FRAMEBUFFER_SIZE
    };

    struct EventArg {
        int width;
        int height;
    };

    static bool init(const char *windowName, int width, int height);
    static GLFWwindow *getWindow();
    static void *registerEvent(Application::EventTypes type, std::function<void(Application::EventArg)> callback);
    static glm::vec2 getViewportSize();

private:
    static GLFWwindow *window;
    static EventDispatcher<EventArg> *eventDispatcher;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void window_focus_callback(GLFWwindow *window, int focused);
};

#endif
