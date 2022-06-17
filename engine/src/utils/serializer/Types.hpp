#pragma once

#include "utils/serializer/TypeRegistry.hpp"
#include <exception>
#include <fmt/format.h>
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

    class ParseError : public std::exception {
    public:
        ParseError(std::size_t start, std::size_t end, std::string_view message, std::string_view file) {
            std::size_t line_begin = 0;
            std::size_t line_end = 0;
            std::size_t line_nr = 0;

            for (std::size_t i = 0; i < file.length(); ++i) {
                if (file[i] == '\n') {
                    ++line_nr;
                    if (i < start) {
                        line_begin = i + 1;
                    } else {
                        line_end = i;
                        break;
                    }
                }
            }

            m_message = fmt::format("--------------------\n{red}Error{reset} in line {}:\n{}{red}{}{reset}{}\n{}^- {}",
                line_nr,
                file.substr(line_begin, start - line_begin),
                file.substr(start, end - start),
                file.substr(end, line_end - end),
                std::string(start - line_begin, ' '),
                message,
                fmt::arg("red", "\033[31m"),
                fmt::arg("reset", "\033[0m"));
        }

        const char* what() const noexcept override {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class Parser {
    public:
        Parser(std::string content)
            : m_content(content)
            , m_pos(0) { }
        virtual ~Parser() = default;
        virtual Value parse() = 0;

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
