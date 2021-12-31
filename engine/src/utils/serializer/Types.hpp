#pragma once

#include "utils/serializer/TypeRegistry.hpp"
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace Birdy3d::serializer {

    struct String;
    struct Number;
    struct Bool;
    struct Null;
    struct Array;
    struct Object;

    // Types
    using Value = std::variant<Null, String, Number, Bool, Array, Object>;

    struct Null { };

    struct String {
        String(std::string v)
            : value(v) { }
        std::string value;
    };

    struct Number {
        Number(double v)
            : value(v) { }
        double value;
    };

    struct Bool {
        Bool(bool v)
            : value(v) { }
        bool value;
    };

    struct Array {
        std::vector<Value> value;

        Value& operator[](std::size_t i) {
            return value[i];
        }
    };

    struct Object {
        std::map<std::string, Value> value;

        Value& operator[](std::string i) {
            return value[i];
        }
    };

    // Parser
    class Parser {
    public:
        Parser(std::string content)
            : m_content(content)
            , m_pos(0) { }
        virtual ~Parser() = default;
        virtual std::optional<Value> parse() = 0;

    protected:
        std::string m_content;
        std::size_t m_pos;

        // Utils
        char consume_char() {
            if (m_pos >= m_content.size())
                return '\0';
            return m_content[m_pos++];
        };

        char current_char() {
            if (m_pos >= m_content.size())
                return '\0';
            return m_content[m_pos];
        };

        std::string consume_until(char until) {
            std::string out;
            char c = consume_char();
            while (c != until) {
                if (c == '\0')
                    return {};
                out += c;
                c = consume_char();
            };
            m_pos--;
            return out;
        }

        bool match(std::string s) {
            std::size_t begin = m_pos;
            std::size_t i = 0;
            char c = consume_char();
            while (i < s.length() && c == s[i]) {
                c = consume_char();
                i++;
            }
            if (i == s.length()) {
                m_pos--;
                return true;
            }
            m_pos = begin;
            return false;
        }
    };

    // Generator
    class Generator {
    public:
        Generator(std::ostream& stream)
            : m_stream(stream) { }
        virtual ~Generator() = default;
        virtual void generate(const Value&) = 0;

    protected:
        std::ostream& m_stream;
    };

}
