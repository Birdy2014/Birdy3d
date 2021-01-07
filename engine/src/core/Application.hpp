#ifndef BIRDY3D_ENGINE_HPP
#define BIRDY3D_ENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/EventDispatcher.hpp"
#include "ui/TextRenderer.hpp"

class Application {
public:
    enum EventTypes {
        EVENT_FRAMEBUFFER_SIZE
    };

    struct EventArg {
        int width;
        int height;
    };

    static bool init(const char *windowName, int width, int height, const std::string &font, unsigned int fontSize);
    static GLFWwindow *getWindow();
    static void *registerEvent(Application::EventTypes type, std::function<void(Application::EventArg)> callback);
    static glm::vec2 getViewportSize();
    static TextRenderer *getTextRenderer();

private:
    static GLFWwindow *window;
    static EventDispatcher<EventArg> *eventDispatcher;
    static TextRenderer *textRenderer;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void window_focus_callback(GLFWwindow *window, int focused);
};

#endif
