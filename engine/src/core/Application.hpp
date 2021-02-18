#pragma once

#include "core/EventDispatcher.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class TextRenderer;
    class Theme;

    class Application {
    public:
        enum EventTypes {
            EVENT_FRAMEBUFFER_SIZE
        };

        struct EventArg {
            int width;
            int height;
        };

        static Theme* defaultTheme;

        static bool init(const char* windowName, int width, int height, const std::string& font, unsigned int fontSize);
        static GLFWwindow* getWindow();
        static void* registerEvent(Application::EventTypes type, std::function<void(Application::EventArg)> callback);
        static glm::vec2 getViewportSize();
        static TextRenderer* getTextRenderer();

    private:
        static GLFWwindow* window;
        static EventDispatcher<EventArg>* eventDispatcher;
        static TextRenderer* textRenderer;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_focus_callback(GLFWwindow* window, int focused);
    };

}
