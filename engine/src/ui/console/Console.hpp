#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class Window;
    class TextField;

    typedef std::function<void(std::vector<std::string>)> CommandCallback;

    class Console {
    public:
        /**
         * @brief Attaches the console to a canvas.
         * @param canvas Canvas to be attached to.
         */
        static void attach(Canvas&);

        /**
         * @brief Prints text to the console.
         * @param text The text to print.
         * @param color The color in which the text is printed. NONE won't change the color.
         */
        static void print(const std::string&, utils::Color::Name = utils::Color::Name::NONE);

        /**
         * @brief Prints line of text to the console.
         * @param text The text to print.
         * @param color The color in which the text is printed. NONE won't change the color. The color will be reset to FG after the line is printed.
         */
        static void println(const std::string&, utils::Color::Name = utils::Color::Name::NONE);

        /**
         * @brief Executes a command in the console and prints the command.
         * @param input The command to be executed including space-separated arguments.
         */
        static void exec(std::string);

        /**
         * @brief Executes a command in the console.
         * @param name The name of the command.
         * @param args A vector of arguments.
         */
        static void exec(std::string name, std::vector<std::string> args);

        /**
         * @brief Register a new command to the console.
         * @param name Name of the command. Any previously registered commands with the same name will be overwritten.
         */
        static void register_command(const std::string& name, const CommandCallback&);

    private:
        // This is required for console.list_commands
        friend class ConsoleCommands;

        static std::map<std::string, CommandCallback> m_commands;
        static std::shared_ptr<Window> m_console_window;
        static std::shared_ptr<TextField> m_console_output;
        static std::shared_ptr<TextField> m_console_input;
        static bool m_created;

        static void create_window();
        static void input_callback();
    };

}
