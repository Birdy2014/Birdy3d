#include "utils/Color.hpp"

#include "core/Logger.hpp"
#include <iomanip>
#include <regex>
#include <sstream>

namespace Birdy3d::utils {

    Color::Color()
    {
        value = glm::vec4(0, 0, 0, 1);
    }

    Color::Color(std::string const& color)
    {
        value = parse(color);
    }

    Color::Color(char const* color)
    {
        value = parse(std::string(color));
    }

    Color::Color(glm::vec4 color)
    {
        value = color;
    }

    Color::operator glm::vec4() const
    {
        return value;
    }

    std::string Color::to_string() const
    {
        std::stringstream stream;
        stream << '#' << std::setfill('0') << std::hex;
        stream << std::setw(2) << std::clamp((int)(value.r * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.g * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.b * 255), 0, 255);
        if ((int)(value.a * 255) > 0)
            stream << std::clamp((int)(value.a * 255), 0, 255);
        return stream.str();
    }

    Color& Color::operator=(std::string const& color)
    {
        value = parse(color);
        return *this;
    }

    bool Color::operator<(Color const& other) const
    {
        float val = value.r * 255 + value.g * 255 * 16 + value.b * 255 * 16 * 16 + value.a * 255 * 16 * 16 * 16;
        float otherval = other.value.r * 255 + other.value.g * 255 * 16 + other.value.b * 255 * 16 * 16 + other.value.a * 255 * 16 * 16 * 16;
        return val < otherval;
    }

    glm::vec4 Color::parse(std::string const& color_string)
    {
        static std::regex const COLOR_REGEX("#?([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})?");

        std::smatch matches;
        if (!std::regex_match(color_string, matches, COLOR_REGEX)) {
            core::Logger::warn("Invalid color: {}", color_string);
            return glm::vec4(1);
        }

        glm::vec4 color(1);
        for (std::size_t i = 1; i < matches.size(); ++i) {
            if (!matches[i].matched)
                continue;
            auto nr = std::stoi(matches[i], nullptr, 16);
            color[i - 1] = nr / 255.0f;
        }

        return color;
    }

    Color const Color::NONE = Color("#00000000");
    Color const Color::WHITE = Color("#FFFFFFFF");
    Color const Color::BLACK = Color("#000000FF");

}

namespace Birdy3d::serializer {

    template <>
    Value adapter_save(utils::Color& value)
    {
        return String(value.to_string());
    }

    template <>
    void adapter_load(Value* from, utils::Color& to)
    {
        if (auto string_ptr = std::get_if<String>(from)) {
            to = utils::Color::parse(string_ptr->value);
        }
    }

}
