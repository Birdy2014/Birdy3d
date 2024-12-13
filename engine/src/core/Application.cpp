#include "core/Application.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Scene.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "render/Rendertarget.hpp"
#include "ui/Canvas.hpp"
#include "ui/Theme.hpp"
#include "ui/console/Commands.hpp"
#include "utils/Stacktrace.hpp"
#include <memory>

namespace Birdy3d::core {

    ResourceHandle<ui::Theme> Application::m_theme;
    std::unique_ptr<events::EventBus> Application::event_bus = {};
    float Application::delta_time = 0;
    std::weak_ptr<ecs::Scene> Application::scene;
    std::weak_ptr<ui::Canvas> Application::canvas;
    ecs::Entity* Application::selected_entity = nullptr;
    GLFWwindow* Application::m_window = nullptr;
    std::unordered_map<BoolOption, bool> Application::m_options_bool;
    std::unordered_map<IntOption, int> Application::m_options_int;

    Channel<std::function<void()>> Application::m_channel_main;
    Channel<std::function<void()>> Application::m_channel_loading;
    std::vector<std::thread> Application::m_loading_threads;

    void glfw_error_callback([[maybe_unused]] int error, char const* description)
    {
        core::Logger::error("GLFW Error: {}", description);
    }

    bool Application::init(char const* window_name, int width, int height, std::string const& theme_name)
    {
#ifdef _GLFW_WAYLAND
        // make wayland default instead of X11
        auto const wayland_display = std::getenv("WAYLAND_DISPLAY");
        if (wayland_display && std::strlen(wayland_display) > 0) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        }
#endif

        if (glfwInit() == GLFW_FALSE) {
            Logger::critical("glfwInit failed");
        }
        glfwSetErrorCallback(glfw_error_callback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHintString(GLFW_X11_CLASS_NAME, "Birdy3d");
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, "Birdy3d");

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

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_message_callback, 0);

        // Init variables
        event_bus = std::make_unique<events::EventBus>();
        if (!theme(theme_name))
            Logger::critical("Invalid Theme '{}'", theme_name);
        ui::ConsoleCommands::register_all();
        render::Rendertarget::DEFAULT = std::shared_ptr<render::Rendertarget>(new render::Rendertarget(width, height, 0));
        option_bool(BoolOption::VSYNC, true);
        option_int(IntOption::SHADOW_CASCADE_SIZE, 5);

        std::size_t const loading_thread_count = 4;

        for (std::size_t i = 0; i < loading_thread_count; ++i) {
            m_loading_threads.push_back(std::thread([]() {
                try {
                    while (true) {
                        auto task = m_channel_loading.get();
                        std::invoke(task);
                    }
                } catch (ChannelClosedException&) {
                    return;
                }
            }));
        }

        return true;
    }

    void Application::cleanup()
    {
        glfwTerminate();
        m_channel_loading.close();
        for (auto& thread : m_loading_threads) {
            thread.join();
        }
    }

    void Application::mainloop()
    {
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

            while (true) {
                auto task = m_channel_main.try_get();
                if (task.has_value()) {
                    std::invoke(task.value());
                } else {
                    break;
                }
            }

            // draw the entitys
            if (scene_ptr) {
                if (auto camera = scene_ptr->main_camera.lock()) {
                    camera->render();
                    camera->render_outline(selected_entity);
                    if (option_bool(BoolOption::SHOW_COLLIDERS))
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

    void Application::framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        render::Rendertarget::DEFAULT->resize(width, height);
        event_bus->emit<events::WindowResizeEvent>(width, height);
    }

    void Application::window_focus_callback(GLFWwindow* window, int focused)
    {
        if (focused && Input::is_cursor_hidden())
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Application::scroll_callback(GLFWwindow*, double xoffset, double yoffset)
    {
        event_bus->emit<events::InputScrollEvent>(xoffset, yoffset);
    }

    void Application::mouse_button_callback(GLFWwindow*, int button, int action, int mods)
    {
        event_bus->emit<events::InputClickEvent>(button, action, mods);
    }

    void Application::key_callback(GLFWwindow*, int key, int scancode, int action, int mods)
    {
        event_bus->emit<events::InputKeyEvent>(key, scancode, action, mods);
    }

    void Application::character_callback(GLFWwindow*, unsigned int codepoint)
    {
        event_bus->emit<events::InputCharEvent>(codepoint);
    }

    void Application::gl_message_callback(GLenum source, GLenum type, GLenum, GLenum severity, GLsizei, GLchar const* message, void const*)
    {
        static std::unordered_map<GLenum, std::string> const ERROR_SOURCE_MAP{
            {GL_DEBUG_SOURCE_API, "SOURCE_API"},
            {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
            {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
            {GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
            {GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
            {GL_DEBUG_SOURCE_OTHER, "OTHER"}};
        static std::unordered_map<GLenum, std::string> const ERROR_TYPE_MAP{
            {GL_DEBUG_TYPE_ERROR, "ERROR"},
            {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
            {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
            {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
            {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
            {GL_DEBUG_TYPE_OTHER, "OTHER"},
            {GL_DEBUG_TYPE_MARKER, "MARKER"}};
        static std::unordered_map<GLenum, std::string> const ERROR_SEVERITY_MAP{
            {GL_DEBUG_SEVERITY_HIGH, "HIGH"},
            {GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"},
            {GL_DEBUG_SEVERITY_LOW, "LOW"},
            {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"}};

        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
            return;

        utils::print_stacktrace();

        if (type == GL_DEBUG_TYPE_ERROR)
            core::Logger::error("OpenGL: source = {}, severity = {}, message = {}\n", ERROR_SOURCE_MAP.at(source), ERROR_SEVERITY_MAP.at(severity), message);
        else
            core::Logger::warn("OpenGL: source = {}, type = {}, severity = {}, message = {}\n", ERROR_SOURCE_MAP.at(source), ERROR_TYPE_MAP.at(type), ERROR_SEVERITY_MAP.at(severity), message);
    }

    GLFWwindow* Application::get_window()
    {
        return m_window;
    }

    glm::vec2 Application::get_viewport_size()
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return glm::vec2(viewport[2], viewport[3]);
    }

    bool Application::option_bool(BoolOption option)
    {
        return m_options_bool[option];
    }

    void Application::option_toggle(BoolOption option)
    {
        option_bool(option, !option_bool(option));
    }

    void Application::option_bool(BoolOption option, bool value)
    {
        m_options_bool[option] = value;
        switch (option) {
        case BoolOption::VSYNC:
            glfwSwapInterval(value);
            break;
        default:
            break;
        }
    }

    int Application::option_int(IntOption option)
    {
        return m_options_int[option];
    }

    void Application::option_int(IntOption option, int value)
    {
        m_options_int[option] = value;
    }

    ui::Theme const& Application::theme()
    {
        return *m_theme;
    }

    bool Application::theme(std::string const& name)
    {
        auto new_theme = ResourceManager::get_theme(name);
        if (!new_theme)
            return false;
        m_theme = new_theme;
        return true;
    }

    void Application::defer_main(std::function<void()> function)
    {
        m_channel_main.push_task(function);
    }

    void Application::defer_loading(std::function<void()> function)
    {
        m_channel_loading.push_task(function);
    }

}
