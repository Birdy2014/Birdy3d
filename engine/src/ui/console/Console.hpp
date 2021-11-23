#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Window;
    class Textarea;
    class TextField;

    typedef std::function<void(std::vector<std::string>)> CommandCallback;

    class Console {
    public:
        static void attach(Canvas&);
        static void print(const std::string&);
        static void println(const std::string&);
        static void exec(std::string);
        static void exec(std::string name, std::vector<std::string> args);
        static void register_command(const std::string& name, const CommandCallback&);

    private:
        // This is required for console.list_commands
        friend class ConsoleCommands;

        static std::map<std::string, CommandCallback> m_commands;
        static std::shared_ptr<Window> m_console_window;
        static std::shared_ptr<Textarea> m_console_output;
        static std::shared_ptr<TextField> m_console_input;

        static void create_window();
        static void input_callback();
    };

}
