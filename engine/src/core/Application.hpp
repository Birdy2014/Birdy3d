#pragma once

#include "core/Base.hpp"
#include "core/Forward.hpp"
#include "ecs/Forward.hpp"
#include "events/Forward.hpp"
#include "ui/Forward.hpp"

namespace Birdy3d::core {

    enum class BoolOption {
        VSYNC,
        SHOW_COLLIDERS
    };

    enum class IntOption {
        SHADOW_CASCADE_SIZE
    };

    class Application {
    public:
        static std::unique_ptr<events::EventBus> event_bus;
        static float delta_time;
        static std::weak_ptr<ecs::Scene> scene;
        static std::weak_ptr<ui::Canvas> canvas;
        static ecs::Entity* selected_entity;

        static bool init(const char* window_name, int width, int height, const std::string& theme_name);
        static void cleanup();
        static void mainloop();
        static GLFWwindow* get_window();
        static glm::vec2 get_viewport_size();
        static bool option_bool(BoolOption);
        static void option_toggle(BoolOption);
        static void option_bool(BoolOption, bool);
        static int option_int(IntOption);
        static void option_int(IntOption, int);
        static ui::Theme& theme();
        static bool theme(const std::string&);

    private:
        static GLFWwindow* m_window;
        static std::unordered_map<BoolOption, bool> m_options_bool;
        static std::unordered_map<IntOption, int> m_options_int;
        static ResourceHandle<ui::Theme> m_theme;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_focus_callback(GLFWwindow* window, int focused);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void character_callback(GLFWwindow* window, unsigned int codepoint);
        static void gl_message_callback(GLenum, GLenum, GLenum, GLenum, GLsizei, const GLchar*, const void*);
    };

}
