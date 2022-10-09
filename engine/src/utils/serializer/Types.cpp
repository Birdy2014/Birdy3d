#include "utils/serializer/Types.hpp"

namespace Birdy3d::serializer {

    Value& Array::operator[](std::size_t i) {
        return value[i];
    }

    Value& Object::operator[](std::string i) {
        return value[i];
    }

    ParseError::ParseError(std::size_t start, std::size_t end, std::string_view message, std::string_view file) {
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

    char Parser::consume_char() {
        if (m_pos >= m_content.size())
            return '\0';
        return m_content[m_pos++];
    };

    char Parser::current_char() {
        if (m_pos >= m_content.size())
            return '\0';
        return m_content[m_pos];
    };

    std::string Parser::consume_until(char until) {
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

    bool Parser::match(std::string s) {
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

}
