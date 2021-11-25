#include "ui/console/Commands.hpp"
#include "ui/console/Console.hpp"

namespace Birdy3d {

    void ConsoleCommands::register_ui() {
        Console::register_command("ui.theme", [](std::vector<std::string> args) {
            if (args.size() != 1) {
                Console::println("Usage: ui.theme <theme>");
                return;
            }
            if (!Application::theme(args[0])) {
                Console::println("error loading theme " + args[0]);
                return;
            }
            Console::println("applied theme " + args[0]);
        });
    }

}
