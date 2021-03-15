#include "core/Application.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "events/InputClickEvent.hpp"
#include "events/InputScrollEvent.hpp"
#include "events/WindowResizeEvent.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Theme* Application::defaultTheme = nullptr;
    EventBus* Application::eventBus = nullptr;
    float Application::deltaTime = 0;
    GLFWwindow* Application::window = nullptr;
    TextRenderer* Application::textRenderer = nullptr;

    bool Application::init(const char* windowName, int width, int height, const std::string& font, unsigned int fontSize) {
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

        // Init variables
        eventBus = new EventBus();
        textRenderer = new TextRenderer(RessourceManager::getFontPath(font), fontSize);

        return true;
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

    GLFWwindow* Application::getWindow() {
        return window;
    }

    glm::vec2 Application::getViewportSize() {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return glm::vec2(viewport[2], viewport[3]);
    }

    TextRenderer* Application::getTextRenderer() {
        return textRenderer;
    }

}
