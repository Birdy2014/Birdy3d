#pragma once

namespace Birdy3d {

    class ConsoleCommands {
    public:
        static void register_all() {
            register_console();
            register_ui();
        }

    private:
        static void register_console();
        static void register_ui();
    };

}
