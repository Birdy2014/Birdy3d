#include "ui/console/Commands.hpp"
#include "ui/console/Console.hpp"

namespace Birdy3d {

    void ConsoleCommands::register_ui() {
        Console::register_command("ui.theme", [](std::vector<std::string> args) {
            if (args.size() != 1) {
                Console::println("Usage: ui.theme <theme>");
                return;
            }
            auto theme = ResourceManager::get_theme(args[0]);
            if (!theme) {
                Console::println("can't find theme " + args[0]);
                return;
            }
            Application::theme = theme;
            Console::println("applied theme " + args[0]);
        });
    }

}
