#include "core/Application.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "ecs/Scene.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "ui/Canvas.hpp"
#include "ui/Theme.hpp"
#include "ui/console/Commands.hpp"

namespace Birdy3d::core {

    ResourceHandle<ui::Theme> Application::m_theme;
    events::EventBus* Application::event_bus = nullptr;
    float Application::delta_time = 0;
    std::weak_ptr<ecs::Scene> Application::scene;
    std::weak_ptr<ui::Canvas> Application::canvas;
    ecs::Entity* Application::selected_entity = nullptr;
    GLFWwindow* Application::m_window = nullptr;
    std::unordered_map<Option, bool> Application::m_options_bool;

    bool Application::init(const char* window_name, int width, int height, const std::string& theme_name) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create Window
        Application::m_window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
        if (m_window == nullptr) {
            Logger::critical("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(m_window);

        // Load OpenGL
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Logger::critical("Failed to initialize GLAD");
            return false;
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set Viewport and resize callback
        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetWindowFocusCallback(m_window, window_focus_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetCharCallback(m_window, character_callback);

        // Init variables
        ResourceManager::init();
        event_bus = new events::EventBus();
        if (!theme(theme_name))
            Logger::critical("Invalid Theme");
        ui::ConsoleCommands::register_all();

        return true;
    }

    void Application::cleanup() {
        glfwTerminate();
    }

    void Application::mainloop() {
        float last_frame = 0.0f;
        while (!glfwWindowShouldClose(m_window)) {
            float current_frame = glfwGetTime();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;

            auto scene_ptr = scene.lock();
            auto canvas_ptr = canvas.lock();

            Input::update();

            if (canvas_ptr)
                canvas_ptr->update();

            if (scene_ptr)
                scene_ptr->update();

            event_bus->flush();

            // draw the entitys
            if (scene_ptr) {
                if (auto camera = scene_ptr->main_camera.lock()) {
                    camera->render();
                    camera->render_outline(selected_entity);
                    if (option_bool(Option::SHOW_COLLIDERS))
                        camera->render_collider_wireframe();
                }
            }

            if (canvas_ptr)
                canvas_ptr->draw_canvas();

            if (scene_ptr)
                scene_ptr->post_update();

            // swap Buffers
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }

    void Application::framebuffer_size_callback(GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        event_bus->emit<events::WindowResizeEvent>(width, height);
    }

    void Application::window_focus_callback(GLFWwindow* window, int focused) {
        if (focused && Input::is_cursor_hidden())
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Application::scroll_callback(GLFWwindow*, double xoffset, double yoffset) {
        event_bus->emit<events::InputScrollEvent>(xoffset, yoffset);
    }

    void Application::mouse_button_callback(GLFWwindow*, int button, int action, int mods) {
        event_bus->emit<events::InputClickEvent>(button, action, mods);
    }

    void Application::key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
        event_bus->emit<events::InputKeyEvent>(key, scancode, action, mods);
    }

    void Application::character_callback(GLFWwindow*, unsigned int codepoint) {
        event_bus->emit<events::InputCharEvent>(codepoint);
    }

    GLFWwindow* Application::get_window() {
        return m_window;
    }

    glm::vec2 Application::get_viewport_size() {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return glm::vec2(viewport[2], viewport[3]);
    }

    bool Application::option_bool(Option option) {
        return m_options_bool[option];
    }

    void Application::option_toggle(Option option) {
        option_bool(option, !option_bool(option));
    }

    void Application::option_bool(Option option, bool value) {
        m_options_bool[option] = value;
        switch (option) {
        case Option::VSYNC:
            glfwSwapInterval(value);
            break;
        default:
            break;
        }
    }

    ui::Theme& Application::theme() {
        return *m_theme;
    }

    bool Application::theme(const std::string& name) {
        auto new_theme = ResourceManager::get_theme(name);
        if (!new_theme)
            return false;
        m_theme = new_theme;
        return true;
    }

}
