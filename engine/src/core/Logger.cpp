#include "core/Logger.hpp"

#include "ui/console/Console.hpp"

namespace Birdy3d::core {

    void Logger::print_console(const std::string& text, Type type) {
        utils::Color::Name color = [&type] {
            switch (type) {
            case Type::DEBUG:
                return utils::Color::Name::NONE;
            case Type::WARN:
                return utils::Color::Name::YELLOW;
            case Type::ERROR:
                return utils::Color::Name::RED;
            }
        }();
        ui::Console::println(text, color);
    }

}
