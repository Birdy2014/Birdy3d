#include "core/Logger.hpp"

#include "ui/console/Console.hpp"

namespace Birdy3d {

    void Logger::print_console(const std::string& text, Type type) {
        Color::Name color = [&type] {
            switch (type) {
            case Type::DEBUG:
                return Color::Name::NONE;
            case Type::WARN:
                return Color::Name::YELLOW;
            case Type::ERROR:
                return Color::Name::RED;
            }
        }();
        Console::println(text, color);
    }

}
