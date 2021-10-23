#include "utils/serializer/Json.hpp"

namespace Birdy3d::serializer {

    std::unique_ptr<Value> JsonParser::parse() {
        if (auto value = parse_string())
            return value;
        if (auto value = parse_number())
            return value;
        if (auto value = parse_bool())
            return value;
        if (auto value = parse_null())
            return value;
        if (auto value = parse_array())
            return value;
        if (auto value = parse_object())
            return value;
        return nullptr;
    }

    std::unique_ptr<String> JsonParser::parse_string() {
        std::size_t begin = m_pos;
        forward();
        char c = consume_char();
        if (c != '"' && c != '\'') {
            m_pos = begin;
            return nullptr;
        }
        std::string s = consume_until(c);
        if (current_char() == '\0') {
            m_pos = begin;
            return nullptr;
        }
        m_pos++;
        return std::make_unique<String>(s);
    }

    std::unique_ptr<Number> JsonParser::parse_number() {
        std::size_t begin = m_pos;
        forward();
        bool dot = false;
        std::string num;
        char c = consume_char();
        if (!((c >= '0' && c <= '9') || c == '.' || c == '-')) {
            m_pos = begin;
            return nullptr;
        }
        while ((c >= '0' && c <= '9') || c == '.' || c == '-') {
            if (c == '.') {
                if (dot) {
                    m_pos = begin;
                    return nullptr;
                }
                dot = true;
            }
            num += c;
            c = consume_char();
        }
        m_pos--;
        return std::make_unique<Number>(std::atof(num.c_str()));
    }

    std::unique_ptr<Bool> JsonParser::parse_bool() {
        std::size_t begin = m_pos;
        forward();
        if (match("true"))
            return std::make_unique<Bool>(true);
        if (match("false"))
            return std::make_unique<Bool>(false);
        m_pos = begin;
        return nullptr;
    }

    std::unique_ptr<Null> JsonParser::parse_null() {
        std::size_t begin = m_pos;
        forward();
        if (match("null"))
            return std::make_unique<Null>();
        m_pos = begin;
        return nullptr;
    }

    std::unique_ptr<Array> JsonParser::parse_array() {
        std::size_t begin = m_pos;
        forward();
        if (consume_char() != '[') {
            m_pos = begin;
            return nullptr;
        }
        auto array = std::make_unique<Array>();
        do {
            forward();
            if (current_char() == ']') {
                m_pos++;
                break;
            }
            if (auto value = parse_string()) {
                array->value.push_back(std::move(value));
                continue;
            }
            if (auto value = parse_number()) {
                array->value.push_back(std::move(value));
                continue;
            }
            if (auto value = parse_bool()) {
                array->value.push_back(std::move(value));
                continue;
            }
            if (auto value = parse_null()) {
                array->value.push_back(std::move(value));
                continue;
            }
            if (auto value = parse_array()) {
                array->value.push_back(std::move(value));
                continue;
            }
            if (auto value = parse_object()) {
                array->value.push_back(std::move(value));
                continue;
            }
            m_pos = begin;
            return nullptr;
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (consume_char() != ']') {
            m_pos = begin;
            return nullptr;
        }
        return array;
    }

    std::unique_ptr<Object> JsonParser::parse_object() {
        std::size_t begin = m_pos;
        forward();
        if (consume_char() != '{') {
            m_pos = begin;
            return nullptr;
        }
        auto object = std::make_unique<Object>();
        do {
            forward();
            if (current_char() == '}') {
                m_pos++;
                break;
            }
            auto key = parse_string();
            if (!key) {
                m_pos = begin;
                return nullptr;
            }
            forward();
            if (consume_char() != ':') {
                m_pos = begin;
                return nullptr;
            }
            forward();
            if (auto value = parse_string()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            if (auto value = parse_number()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            if (auto value = parse_bool()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            if (auto value = parse_null()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            if (auto value = parse_array()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            if (auto value = parse_object()) {
                object->value[key->value] = std::move(value);
                continue;
            }
            m_pos = begin;
            return nullptr;
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (consume_char() != '}') {
            m_pos = begin;
            return nullptr;
        }
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
    void MinimalJsonGenerator::generate(Value& value) {
        if (auto v = dynamic_cast<String*>(&value))
            return generate_string(*v);
        if (auto v = dynamic_cast<Number*>(&value))
            return generate_number(*v);
        if (auto v = dynamic_cast<Bool*>(&value))
            return generate_bool(*v);
        if (auto v = dynamic_cast<Null*>(&value))
            return generate_null(*v);
        if (auto v = dynamic_cast<Array*>(&value))
            return generate_array(*v);
        if (auto v = dynamic_cast<Object*>(&value))
            return generate_object(*v);
    }

    void MinimalJsonGenerator::generate_string(String& value) {
        m_stream << '"' << value.value << '"';
    }

    void MinimalJsonGenerator::generate_number(Number& value) {
        m_stream << value.value;
    }

    void MinimalJsonGenerator::generate_bool(Bool& value) {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void MinimalJsonGenerator::generate_null(Null&) {
        m_stream << "null";
    }

    void MinimalJsonGenerator::generate_array(Array& value) {
        m_stream << '[';
        for (std::size_t i = 0; i < value.value.size(); i++) {
            auto item = value.value[i].get();
            if (auto v = dynamic_cast<String*>(item))
                generate_string(*v);
            else if (auto v = dynamic_cast<Number*>(item))
                generate_number(*v);
            else if (auto v = dynamic_cast<Bool*>(item))
                generate_bool(*v);
            else if (auto v = dynamic_cast<Null*>(item))
                generate_null(*v);
            else if (auto v = dynamic_cast<Array*>(item))
                generate_array(*v);
            else if (auto v = dynamic_cast<Object*>(item))
                generate_object(*v);
            if (i < value.value.size() - 1)
                m_stream << ',';
        }
        m_stream << ']';
    }

    void MinimalJsonGenerator::generate_object(Object& value) {
        m_stream << '{';
        std::size_t i = 0;
        for (const auto& pair : value.value) {
            m_stream << '"' << pair.first << "\":";
            auto item = pair.second.get();
            if (auto v = dynamic_cast<String*>(item))
                generate_string(*v);
            else if (auto v = dynamic_cast<Number*>(item))
                generate_number(*v);
            else if (auto v = dynamic_cast<Bool*>(item))
                generate_bool(*v);
            else if (auto v = dynamic_cast<Null*>(item))
                generate_null(*v);
            else if (auto v = dynamic_cast<Array*>(item))
                generate_array(*v);
            else if (auto v = dynamic_cast<Object*>(item))
                generate_object(*v);
            if (i < value.value.size() - 1)
                m_stream << ',';
            i++;
        }
        m_stream << '}';
    }

    // Pretty Generator
    void PrettyJsonGenerator::generate(Value& value) {
        if (auto v = dynamic_cast<String*>(&value))
            return generate_string(*v);
        if (auto v = dynamic_cast<Number*>(&value))
            return generate_number(*v);
        if (auto v = dynamic_cast<Bool*>(&value))
            return generate_bool(*v);
        if (auto v = dynamic_cast<Null*>(&value))
            return generate_null(*v);
        if (auto v = dynamic_cast<Array*>(&value))
            return generate_array(*v, 0);
        if (auto v = dynamic_cast<Object*>(&value))
            return generate_object(*v, 0);
    }

    void PrettyJsonGenerator::generate_string(String& value) {
        m_stream << '"' << value.value << '"';
    }

    void PrettyJsonGenerator::generate_number(Number& value) {
        m_stream << value.value;
    }

    void PrettyJsonGenerator::generate_bool(Bool& value) {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void PrettyJsonGenerator::generate_null(Null&) {
        m_stream << "null";
    }

    void PrettyJsonGenerator::generate_array(Array& value, std::size_t indent) {
        m_stream << '[';
        for (std::size_t i = 0; i < value.value.size(); i++) {
            m_stream << '\n';
            for (std::size_t i = 0; i < indent * m_indent_width + m_indent_width; i++)
                m_stream << ' ';
            auto item = value.value[i].get();
            if (auto v = dynamic_cast<String*>(item))
                generate_string(*v);
            else if (auto v = dynamic_cast<Number*>(item))
                generate_number(*v);
            else if (auto v = dynamic_cast<Bool*>(item))
                generate_bool(*v);
            else if (auto v = dynamic_cast<Null*>(item))
                generate_null(*v);
            else if (auto v = dynamic_cast<Array*>(item))
                generate_array(*v, indent + 1);
            else if (auto v = dynamic_cast<Object*>(item))
                generate_object(*v, indent + 1);
            if (i < value.value.size() - 1)
                m_stream << ',';
        }
        m_stream << '\n';
        for (std::size_t i = 0; i < indent * m_indent_width; i++)
            m_stream << ' ';
        m_stream << ']';
    }

    void PrettyJsonGenerator::generate_object(Object& value, std::size_t indent) {
        m_stream << '{';
        std::size_t i = 0;
        for (const auto& pair : value.value) {
            m_stream << '\n';
            for (std::size_t i = 0; i < indent * m_indent_width + m_indent_width; i++)
                m_stream << ' ';
            m_stream << '"' << pair.first << "\": ";
            auto item = pair.second.get();
            if (auto v = dynamic_cast<String*>(item))
                generate_string(*v);
            else if (auto v = dynamic_cast<Number*>(item))
                generate_number(*v);
            else if (auto v = dynamic_cast<Bool*>(item))
                generate_bool(*v);
            else if (auto v = dynamic_cast<Null*>(item))
                generate_null(*v);
            else if (auto v = dynamic_cast<Array*>(item))
                generate_array(*v, indent + 1);
            else if (auto v = dynamic_cast<Object*>(item))
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
