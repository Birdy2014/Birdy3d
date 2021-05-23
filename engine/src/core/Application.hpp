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

        static bool init(const char* windowName, int width, int height);
        static void cleanup();
        static GLFWwindow* getWindow();
        static glm::vec2 getViewportSize();
        static TextRenderer* getTextRenderer();

    private:
        static GLFWwindow* window;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_focus_callback(GLFWwindow* window, int focused);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void character_callback(GLFWwindow* window, unsigned int codepoint);
    };

}
