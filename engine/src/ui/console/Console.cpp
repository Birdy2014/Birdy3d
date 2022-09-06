#include "ui/console/Console.hpp"

#include "ui/Canvas.hpp"
#include "ui/DirectionalLayout.hpp"
#include "ui/Window.hpp"
#include "ui/console/Commands.hpp"
#include "ui/widgets/TextField.hpp"
#include <numeric>

namespace Birdy3d::ui {

    std::map<std::string, CommandCallback> Console::m_commands;
    std::shared_ptr<Window> Console::m_console_window;
    std::shared_ptr<TextField> Console::m_console_output;
    std::shared_ptr<TextField> Console::m_console_input;
    bool Console::m_created = false;

    void Console::attach(Canvas& canvas) {
        if (!m_console_window)
            create_window();
        if (!canvas.get_widget("ConsoleWindow", true))
            canvas.add_child(m_console_window);
        // TODO: Allow attaching console to multiple canvases
    }

    void Console::create_window() {
        m_console_window = std::make_shared<Window>(Widget::Options { .size = 400_px, .name = "ConsoleWindow" });
        m_console_window->set_layout<DirectionalLayout>(DirectionalLayout::Direction::DOWN, 5, false);
        m_console_window->title("Console");
        m_console_window->callback_close = []() {
            m_console_window->hidden = true;
        };

        m_console_output = m_console_window->add_child<TextField>({ .size = UIVector(100_p, 0_px), .placement = Placement::TOP_LEFT });
        m_console_output->multiline = true;
        m_console_output->readonly = true;

        m_console_input = m_console_window->add_child<TextField>({ .size = UIVector(100_p, 20_px), .weight = 0 });
        m_console_input->on_accept = input_callback;

        m_created = true;
    }

    void Console::input_callback() {
        std::string input = m_console_input->text();
        m_console_input->clear();
        exec(input);
    }

    void Console::print(const std::string& text, utils::Color::Name color) {
        if (!m_created)
            return;

        std::string color_string;
        if (color != utils::Color::Name::NONE) {
            color_string = '\x1B';
            color_string += (char32_t)color;
        }
        m_console_output->append(color_string + text);
    }

    void Console::println(const std::string& text, utils::Color::Name color) {
        if (!m_created)
            return;

        print(text, color);
        if (color != utils::Color::Name::NONE)
            print("\n", utils::Color::Name::FG);
        else
            print("\n");
        m_console_output->scroll_down();
    }

    void Console::exec(std::string input) {
        if (input.empty())
            return;

        println("> " + input);

        // Split spaces
        std::vector<std::string> command;
        bool in_quotes = false;
        std::string current_segment;
        for (char c : input) {
            if (c == ' ' && !in_quotes) {
                if (current_segment.empty())
                    continue;
                command.push_back(current_segment);
                current_segment.clear();
                continue;
            }
            if (c == '"') {
                in_quotes = !in_quotes;
                continue;
            }
            current_segment += c;
        }
        if (!current_segment.empty()) {
            command.push_back(current_segment);
            current_segment.clear();
        }

        std::string name = command[0];
        command.erase(command.begin());

        // Execute command
        exec(name, command);
    }

    void Console::exec(std::string name, std::vector<std::string> args) {
        if (!m_commands.contains(name)) {
            println("Command not found");
            return;
        }
        m_commands[name](args);
    }

    void Console::register_command(const std::string& name, const CommandCallback& callback) {
        m_commands[name] = std::move(callback);
    }

    void ConsoleCommands::register_console() {
        Console::register_command("console.log", [](std::vector<std::string> args) {
            Console::println(std::accumulate(args.begin(), args.end(), std::string(), [](const std::string& a, const std::string& b) { return a.empty() ? b : a + " " + b; }));
        });

        Console::register_command("console.warn", [](std::vector<std::string> args) {
            Console::println(std::accumulate(args.begin(), args.end(), std::string(), [](const std::string& a, const std::string& b) { return a.empty() ? b : a + " " + b; }), utils::Color::Name::YELLOW);
        });

        Console::register_command("console.error", [](std::vector<std::string> args) {
            Console::println(std::accumulate(args.begin(), args.end(), std::string(), [](const std::string& a, const std::string& b) { return a.empty() ? b : a + " " + b; }), utils::Color::Name::RED);
        });

        Console::register_command("console.clear", [](std::vector<std::string>) {
            Console::m_console_output->clear();
        });

        Console::register_command("console.list_commands", [](std::vector<std::string>) {
            Console::println("Commands:");
            for (auto& [name, callback] : Console::m_commands) {
                Console::println(name);
            }
        });

        Console::register_command("help", [](std::vector<std::string>) {
            Console::println("Use 'console.list_commands' to get a list of all available commands.");
        });
    }

}
