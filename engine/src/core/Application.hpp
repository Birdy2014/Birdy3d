#pragma once

#include "events/EventBus.hpp"
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
        static Theme* defaultTheme;
        static EventBus* eventBus;
        static float deltaTime;

        static bool init(const char* windowName, int width, int height, const std::string& font, unsigned int fontSize);
        static GLFWwindow* getWindow();
        static glm::vec2 getViewportSize();
        static TextRenderer* getTextRenderer();

    private:
        static GLFWwindow* window;
        static TextRenderer* textRenderer;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_focus_callback(GLFWwindow* window, int focused);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    };

}
