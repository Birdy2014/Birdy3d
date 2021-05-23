#include "core/Application.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Theme* Application::defaultTheme = nullptr;
    EventBus* Application::eventBus = nullptr;
    float Application::deltaTime = 0;
    GLFWwindow* Application::window = nullptr;

    bool Application::init(const char* windowName, int width, int height) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create Window
        Application::window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
        if (window == nullptr) {
            Logger::error("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);

        // Load OpenGL
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Logger::error("Failed to initialize GLAD");
            return false;
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set Viewport and resize callback
        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetWindowFocusCallback(window, window_focus_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCharCallback(window, character_callback);

        // Init variables
        eventBus = new EventBus();

        return true;
    }

    void Application::cleanup() {
        glfwTerminate();
    }

    void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        eventBus->emit(new WindowResizeEvent(width, height));
    }

    void Application::window_focus_callback(GLFWwindow* window, int focused) {
        if (focused && Input::isCursorHidden())
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else if (focused)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        eventBus->emit(new InputScrollEvent(xoffset, yoffset));
    }

    void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        eventBus->emit(new InputClickEvent(button, action, mods));
    }

    void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        eventBus->emit(new InputKeyEvent(key, scancode, action, mods));
    }

    void Application::character_callback(GLFWwindow* window, unsigned int codepoint) {
        eventBus->emit(new InputCharEvent(codepoint));
    }

    GLFWwindow* Application::getWindow() {
        return window;
    }

    glm::vec2 Application::getViewportSize() {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return glm::vec2(viewport[2], viewport[3]);
    }

    TextRenderer* Application::getTextRenderer() {
        return RessourceManager::getTextRenderer(defaultTheme->font);
    }

}
