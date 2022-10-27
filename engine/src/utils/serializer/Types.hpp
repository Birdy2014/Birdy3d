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
            : value(v)
        { }
        std::string value;
    };

    struct Number {
        Number(double v)
            : value(v)
        { }
        double value;
    };

    struct Bool {
        Bool(bool v)
            : value(v)
        { }
        bool value;
    };

    struct Array {
        std::vector<Value> value;

        Value& operator[](std::size_t);
    };

    struct Object {
        std::map<std::string, Value> value;

        Value& operator[](std::string);
    };

    class ParseError : public std::exception {
    public:
        ParseError(std::size_t start, std::size_t end, std::string_view message, std::string_view file);

        [[nodiscard]] char const* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class Parser {
    public:
        Parser(std::string content)
            : m_content(content)
            , m_pos(0)
        { }
        virtual ~Parser() = default;
        virtual Value parse() = 0;

    protected:
        std::string m_content;
        std::size_t m_pos;

        // Utils
        char consume_char();
        char current_char();
        std::string consume_until(char until);
        bool match(std::string s);
    };

    class Generator {
    public:
        Generator(std::ostream& stream)
            : m_stream(stream)
        { }
        virtual ~Generator() = default;
        virtual void generate(Value const&) = 0;

    protected:
        std::ostream& m_stream;
    };

}
