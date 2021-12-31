#include "utils/serializer/Json.hpp"

namespace Birdy3d::serializer {

    std::optional<Value> JsonParser::parse() {
        return parse_value();
    }

    std::optional<Value> JsonParser::parse_value() {
        forward();
        char c = current_char();
        if (c == '"' || c == '\'')
            return parse_string();
        if ((c >= '0' && c <= '9') || c == '-')
            return parse_number();
        if (c == 't' || c == 'f')
            return parse_bool();
        if (c == 'n')
            return parse_null();
        if (c == '[')
            return parse_array();
        if (c == '{')
            return parse_object();
        return {};
    }

    std::optional<String> JsonParser::parse_string() {
        char c = consume_char();
        if (c != '"' && c != '\'')
            return {};
        std::string s = consume_until(c);
        if (current_char() == '\0')
            return {};
        m_pos++;
        return String(s);
    }

    std::optional<Number> JsonParser::parse_number() {
        bool dot = false;
        bool e = false;
        std::string num;
        char c = consume_char();
        if (!((c >= '0' && c <= '9') || c == '.' || c == '-'))
            return {};
        while ((c >= '0' && c <= '9') || c == '.' || c == '-' || c == 'e') {
            if (c == '.') {
                if (dot)
                    return {};
                dot = true;
            }
            if (c == 'e') {
                if (e)
                    return {};
                e = true;
            }
            num += c;
            c = consume_char();
        }
        m_pos--;
        return Number(std::atof(num.c_str()));
    }

    std::optional<Bool> JsonParser::parse_bool() {
        if (match("true"))
            return Bool(true);
        if (match("false"))
            return Bool(false);
        return {};
    }

    std::optional<Null> JsonParser::parse_null() {
        if (match("null"))
            return Null();
        return {};
    }

    std::optional<Array> JsonParser::parse_array() {
        if (consume_char() != '[')
            return {};
        auto array = Array();
        do {
            forward();
            if (current_char() == ']') {
                m_pos++;
                break;
            }

            if (auto value = parse_value()) {
                array.value.push_back(*value);
                continue;
            }
            return {};
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (consume_char() != ']')
            return {};
        return array;
    }

    std::optional<Object> JsonParser::parse_object() {
        if (consume_char() != '{')
            return {};
        auto object = Object();
        do {
            forward();
            if (current_char() == '}') {
                m_pos++;
                break;
            }
            auto key = parse_string();
            if (!key)
                return {};
            forward();
            if (consume_char() != ':')
                return {};
            forward();
            if (auto value = parse_value()) {
                object.value[key->value] = *value;
                continue;
            }
            return {};
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (char c = consume_char() != '}')
            return {};
        return object;
    }

    // Utils
    void JsonParser::forward() {
        char c;
        do {
            c = consume_char();
        } while (c == ' ' || c == '\n');
        m_pos--;
    }

    // Minimal Generator
    void MinimalJsonGenerator::generate(const Value& value) {
        if (auto v = std::get_if<String>(&value))
            return generate_string(*v);
        if (auto v = std::get_if<Number>(&value))
            return generate_number(*v);
        if (auto v = std::get_if<Bool>(&value))
            return generate_bool(*v);
        if (auto v = std::get_if<Null>(&value))
            return generate_null(*v);
        if (auto v = std::get_if<Array>(&value))
            return generate_array(*v);
        if (auto v = std::get_if<Object>(&value))
            return generate_object(*v);
    }

    void MinimalJsonGenerator::generate_string(const String& value) {
        m_stream << '"' << value.value << '"';
    }

    void MinimalJsonGenerator::generate_number(const Number& value) {
        m_stream << value.value;
    }

    void MinimalJsonGenerator::generate_bool(const Bool& value) {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void MinimalJsonGenerator::generate_null(const Null&) {
        m_stream << "null";
    }

    void MinimalJsonGenerator::generate_array(const Array& value) {
        m_stream << '[';
        for (std::size_t i = 0; i < value.value.size(); i++) {
            auto& item = value.value[i];
            if (auto v = std::get_if<String>(&item))
                generate_string(*v);
            else if (auto v = std::get_if<Number>(&item))
                generate_number(*v);
            else if (auto v = std::get_if<Bool>(&item))
                generate_bool(*v);
            else if (auto v = std::get_if<Null>(&item))
                generate_null(*v);
            else if (auto v = std::get_if<Array>(&item))
                generate_array(*v);
            else if (auto v = std::get_if<Object>(&item))
                generate_object(*v);
            if (i < value.value.size() - 1)
                m_stream << ',';
        }
        m_stream << ']';
    }

    void MinimalJsonGenerator::generate_object(const Object& value) {
        m_stream << '{';
        std::size_t i = 0;
        for (const auto& pair : value.value) {
            m_stream << '"' << pair.first << "\":";
            auto& item = pair.second;
            if (auto v = std::get_if<String>(&item))
                generate_string(*v);
            else if (auto v = std::get_if<Number>(&item))
                generate_number(*v);
            else if (auto v = std::get_if<Bool>(&item))
                generate_bool(*v);
            else if (auto v = std::get_if<Null>(&item))
                generate_null(*v);
            else if (auto v = std::get_if<Array>(&item))
                generate_array(*v);
            else if (auto v = std::get_if<Object>(&item))
                generate_object(*v);
            if (i < value.value.size() - 1)
                m_stream << ',';
            i++;
        }
        m_stream << '}';
    }

    // Pretty Generator
    void PrettyJsonGenerator::generate(const Value& value) {
        if (auto v = std::get_if<String>(&value))
            return generate_string(*v);
        if (auto v = std::get_if<Number>(&value))
            return generate_number(*v);
        if (auto v = std::get_if<Bool>(&value))
            return generate_bool(*v);
        if (auto v = std::get_if<Null>(&value))
            return generate_null(*v);
        if (auto v = std::get_if<Array>(&value))
            return generate_array(*v, 0);
        if (auto v = std::get_if<Object>(&value))
            return generate_object(*v, 0);
    }

    void PrettyJsonGenerator::generate_string(const String& value) {
        m_stream << '"' << value.value << '"';
    }

    void PrettyJsonGenerator::generate_number(const Number& value) {
        m_stream << value.value;
    }

    void PrettyJsonGenerator::generate_bool(const Bool& value) {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void PrettyJsonGenerator::generate_null(const Null&) {
        m_stream << "null";
    }

    void PrettyJsonGenerator::generate_array(const Array& value, std::size_t indent) {
        m_stream << '[';
        for (std::size_t i = 0; i < value.value.size(); i++) {
            m_stream << '\n';
            for (std::size_t i = 0; i < indent * m_indent_width + m_indent_width; i++)
                m_stream << ' ';
            auto item = value.value[i];
            if (auto v = std::get_if<String>(&item))
                generate_string(*v);
            else if (auto v = std::get_if<Number>(&item))
                generate_number(*v);
            else if (auto v = std::get_if<Bool>(&item))
                generate_bool(*v);
            else if (auto v = std::get_if<Null>(&item))
                generate_null(*v);
            else if (auto v = std::get_if<Array>(&item))
                generate_array(*v, indent + 1);
            else if (auto v = std::get_if<Object>(&item))
                generate_object(*v, indent + 1);
            if (i < value.value.size() - 1)
                m_stream << ',';
        }
        m_stream << '\n';
        for (std::size_t i = 0; i < indent * m_indent_width; i++)
            m_stream << ' ';
        m_stream << ']';
    }

    void PrettyJsonGenerator::generate_object(const Object& value, std::size_t indent) {
        m_stream << '{';
        std::size_t i = 0;
        for (const auto& pair : value.value) {
            m_stream << '\n';
            for (std::size_t i = 0; i < indent * m_indent_width + m_indent_width; i++)
                m_stream << ' ';
            m_stream << '"' << pair.first << "\": ";
            auto& item = pair.second;
            if (auto v = std::get_if<String>(&item))
                generate_string(*v);
            else if (auto v = std::get_if<Number>(&item))
                generate_number(*v);
            else if (auto v = std::get_if<Bool>(&item))
                generate_bool(*v);
            else if (auto v = std::get_if<Null>(&item))
                generate_null(*v);
            else if (auto v = std::get_if<Array>(&item))
                generate_array(*v, indent + 1);
            else if (auto v = std::get_if<Object>(&item))
                generate_object(*v, indent + 1);
            if (i < value.value.size() - 1)
                m_stream << ',';
            i++;
        }
        m_stream << '\n';
        for (std::size_t i = 0; i < indent * m_indent_width; i++)
            m_stream << ' ';
        m_stream << '}';
    }

}
