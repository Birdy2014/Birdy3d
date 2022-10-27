#include "utils/serializer/Json.hpp"

namespace Birdy3d::serializer {

    Value JsonParser::parse()
    {
        return parse_value();
    }

    Value JsonParser::parse_value()
    {
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
        throw ParseError{m_pos, m_pos + 1, "Expected value", m_content};
    }

    String JsonParser::parse_string()
    {
        auto start = m_pos;
        char c = consume_char();
        if (c != '"' && c != '\'')
            throw ParseError{start, m_pos, "Expected string", m_content};
        std::string s = consume_until(c);
        if (current_char() == '\0')
            throw ParseError{start, m_pos, "Expected string", m_content};
        m_pos++;
        return String(s);
    }

    Number JsonParser::parse_number()
    {
        auto start = m_pos;
        bool dot = false;
        bool e = false;
        std::string num;
        char c = consume_char();
        if (!((c >= '0' && c <= '9') || c == '.' || c == '-'))
            throw ParseError{start, m_pos, "Expected number", m_content};
        while ((c >= '0' && c <= '9') || c == '.' || c == '-' || c == 'e') {
            if (c == '.') {
                if (dot)
                    throw ParseError{start, m_pos, "Too many dots in number", m_content};
                dot = true;
            }
            if (c == 'e') {
                if (e)
                    throw ParseError{start, m_pos, "Too many e in number", m_content};
                e = true;
            }
            num += c;
            c = consume_char();
        }
        m_pos--;
        return Number(std::atof(num.c_str()));
    }

    Bool JsonParser::parse_bool()
    {
        if (match("true"))
            return Bool(true);
        if (match("false"))
            return Bool(false);
        throw ParseError{m_pos, m_pos + 1, "Expected boolean", m_content};
    }

    Null JsonParser::parse_null()
    {
        if (match("null"))
            return Null();
        throw ParseError{m_pos, m_pos + 1, "Expected null", m_content};
    }

    Array JsonParser::parse_array()
    {
        if (consume_char() != '[')
            throw ParseError{m_pos - 1, m_pos, "Expected array start", m_content};
        auto array = Array();
        do {
            forward();
            if (current_char() == ']') {
                m_pos++;
                break;
            }

            array.value.push_back(parse_value());
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (consume_char() != ']')
            throw ParseError{m_pos - 1, m_pos, "Expected array end", m_content};
        return array;
    }

    Object JsonParser::parse_object()
    {
        if (consume_char() != '{')
            throw ParseError{m_pos - 1, m_pos, "Expected object start", m_content};
        auto object = Object();
        do {
            forward();
            if (current_char() == '}') {
                m_pos++;
                break;
            }
            auto key = parse_string();
            forward();
            if (consume_char() != ':')
                throw ParseError{m_pos - 1, m_pos, "Expected ':'", m_content};
            forward();
            auto value = parse_value();
            object.value[key.value] = value;
        } while (consume_char() == ',');
        m_pos--;
        forward();
        if (consume_char() != '}')
            throw ParseError{m_pos - 1, m_pos, "Expected object end", m_content};
        return object;
    }

    // Utils
    void JsonParser::forward()
    {
        char c;
        do {
            c = consume_char();
        } while (c == ' ' || c == '\n');
        m_pos--;
    }

    // Minimal Generator
    void MinimalJsonGenerator::generate(Value const& value)
    {
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

    void MinimalJsonGenerator::generate_string(String const& value)
    {
        m_stream << '"' << value.value << '"';
    }

    void MinimalJsonGenerator::generate_number(Number const& value)
    {
        m_stream << value.value;
    }

    void MinimalJsonGenerator::generate_bool(Bool const& value)
    {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void MinimalJsonGenerator::generate_null(Null const&)
    {
        m_stream << "null";
    }

    void MinimalJsonGenerator::generate_array(Array const& value)
    {
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

    void MinimalJsonGenerator::generate_object(Object const& value)
    {
        m_stream << '{';
        std::size_t i = 0;
        for (auto const& pair : value.value) {
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
    void PrettyJsonGenerator::generate(Value const& value)
    {
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

    void PrettyJsonGenerator::generate_string(String const& value)
    {
        m_stream << '"' << value.value << '"';
    }

    void PrettyJsonGenerator::generate_number(Number const& value)
    {
        m_stream << value.value;
    }

    void PrettyJsonGenerator::generate_bool(Bool const& value)
    {
        if (value.value)
            m_stream << "true";
        else
            m_stream << "false";
    }

    void PrettyJsonGenerator::generate_null(Null const&)
    {
        m_stream << "null";
    }

    void PrettyJsonGenerator::generate_array(Array const& value, std::size_t indent)
    {
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

    void PrettyJsonGenerator::generate_object(Object const& value, std::size_t indent)
    {
        m_stream << '{';
        std::size_t i = 0;
        for (auto const& pair : value.value) {
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
