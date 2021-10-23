#include "utils/Color.hpp"

#include "core/Logger.hpp"
#include <iomanip>
#include <sstream>

namespace Birdy3d {

    Color::Color() {
        value = glm::vec4(0, 0, 0, 1);
    }

    Color::Color(const std::string& color) {
        value = parse(color);
    }

    Color::Color(const char* color) {
        value = parse(std::string(color));
    }

    Color::Color(glm::vec4 color) {
        value = color;
    }

    Color::operator glm::vec4() const {
        return value;
    }

    std::string Color::to_string() const {
        std::stringstream stream;
        stream << '#' << std::setfill('0') << std::hex;
        stream << std::setw(2) << std::clamp((int)(value.r * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.g * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.b * 255), 0, 255);
        if ((int)(value.a * 255) > 0)
            stream << std::clamp((int)(value.a * 255), 0, 255);
        return stream.str();
    }

    Color& Color::operator=(const std::string& color) {
        value = parse(color);
        return *this;
    }

    bool Color::operator<(const Color& other) const {
        float val = value.r * 255 + value.g * 255 * 16 + value.b * 255 * 16 * 16 + value.a * 255 * 16 * 16 * 16;
        float otherval = other.value.r * 255 + other.value.g * 255 * 16 + other.value.b * 255 * 16 * 16 + other.value.a * 255 * 16 * 16 * 16;
        return val < otherval;
    }

    glm::vec4 Color::parse(const std::string& colorString) {
        bool has_hash = colorString.at(0) == '#';

        int color_length;
        if ((int)colorString.length() == 6 + has_hash || (int)colorString.length() == 8 + has_hash) {
            color_length = 2;
        } else if ((int)colorString.length() == 3 + has_hash) {
            color_length = 1;
        } else {
            Logger::warn("Invalid color: ", colorString);
            return glm::vec4(1);
        }

        glm::vec4 color(1);

        for (size_t pos = has_hash; pos < colorString.length(); pos += color_length) {
            int nr = std::stoi(colorString.substr(pos, color_length), nullptr, 16);
            color[pos / color_length] = nr / (std::pow(16, color_length) - 1);
        }
        return color;
    }

    Color const Color::NONE = Color("#00000000");
    Color const Color::WHITE = Color("#FFFFFFFF");
    Color const Color::BLACK = Color("#000000FF");

    namespace serializer {

        template <>
        std::unique_ptr<Value> adapter_save(Color& value) {
            return std::make_unique<String>(value.to_string());
        }

        template <>
        void adapter_load(Value* from, Color& to) {
            if (auto* string_ptr = from->as_string()) {
                to = Color::parse(string_ptr->value);
            }
        }

    }

}
