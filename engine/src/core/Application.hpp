#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Canvas;
    class EventBus;
    class Entity;
    class Scene;
    class TextRenderer;
    class Theme;

    enum class Option {
        VSYNC,
        SHOW_COLLIDERS
    };

    class Application {
    public:
        static EventBus* event_bus;
        static float delta_time;
        static std::weak_ptr<Scene> scene;
        static std::weak_ptr<Canvas> canvas;
        static Entity* selected_entity;

        static bool init(const char* window_name, int width, int height, const std::string& theme_name);
        static void cleanup();
        static void mainloop();
        static GLFWwindow* get_window();
        static glm::vec2 get_viewport_size();
        static bool option_bool(Option);
        static void option_toggle(Option);
        static void option_bool(Option, bool);
        static Theme& theme() { return *m_theme; }
        static bool theme(const std::string&);

    private:
        static GLFWwindow* m_window;
        static std::unordered_map<Option, bool> m_options_bool;
        static std::shared_ptr<Theme> m_theme;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_focus_callback(GLFWwindow* window, int focused);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void character_callback(GLFWwindow* window, unsigned int codepoint);
    };

}
