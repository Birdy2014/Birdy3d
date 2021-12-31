#pragma once

#include "utils/serializer/Types.hpp"

namespace Birdy3d::serializer {

    class JsonParser : public Parser {
    public:
        using Parser::Parser;
        std::optional<Value> parse() override;

    private:
        // Parse Types
        std::optional<Value> parse_value();
        std::optional<String> parse_string();
        std::optional<Number> parse_number();
        std::optional<Bool> parse_bool();
        std::optional<Null> parse_null();
        std::optional<Array> parse_array();
        std::optional<Object> parse_object();

        // Utils
        void forward();
    };

    class MinimalJsonGenerator : public Generator {
    public:
        using Generator::Generator;
        void generate(const Value&) override;

    private:
        void generate_string(const String&);
        void generate_number(const Number&);
        void generate_bool(const Bool&);
        void generate_null(const Null&);
        void generate_array(const Array&);
        void generate_object(const Object&);
    };

    class PrettyJsonGenerator : public Generator {
    public:
        PrettyJsonGenerator(std::ostream& stream, std::size_t indent_width = 4)
            : Generator(stream)
            , m_indent_width(indent_width) { }
        void generate(const Value&) override;

    private:
        std::size_t m_indent_width = 4;

        void generate_string(const String&);
        void generate_number(const Number&);
        void generate_bool(const Bool&);
        void generate_null(const Null&);
        void generate_array(const Array&, std::size_t);
        void generate_object(const Object&, std::size_t);
    };

}
