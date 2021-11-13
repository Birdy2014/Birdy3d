#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Window;
    class Textarea;
    class TextField;

    class Console {
    public:
        static void attach(Canvas&);
        static void print(const std::string&);
        static void println(const std::string&);
        static void exec(std::string);
        static void exec(std::string name, std::vector<std::string> args);

    private:
        static std::shared_ptr<Window> m_console_window;
        static std::shared_ptr<Textarea> m_console_output;
        static std::shared_ptr<TextField> m_console_input;

        static void create_window();
        static void input_callback();
    };

    typedef std::function<void(std::vector<std::string>)> CommandCallback;

    class CommandRegister {
    public:
        CommandRegister(std::string name, CommandCallback callback);

    private:
        friend class Console;
        static std::map<std::string, CommandCallback> m_commands;

        static bool exec(std::string name, std::vector<std::string> args);
    };

}

#define CONCATENATE(s1, s2) s1##s2
#define EXPAND_THEN_CONCATENATE(s1, s2) CONCATENATE(s1, s2)
#ifdef __COUNTER__
    #define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __COUNTER__)
#else
    #define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __LINE__)
#endif // __COUNTER__
#define BIRDY3D_REGISTER_COMMAND(name, callback)                                         \
    namespace Birdy3d::command_register {                                                \
        static ::Birdy3d::CommandRegister UNIQUE_IDENTIFIER(_register)(#name, callback); \
    }
