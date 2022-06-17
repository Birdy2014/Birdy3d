#pragma once

#include "utils/serializer/Types.hpp"

namespace Birdy3d::serializer {

    class JsonParser : public Parser {
    public:
        using Parser::Parser;
        Value parse() override;

    private:
        // Parse Types
        [[nodiscard]] Value parse_value();
        [[nodiscard]] String parse_string();
        [[nodiscard]] Number parse_number();
        [[nodiscard]] Bool parse_bool();
        [[nodiscard]] Null parse_null();
        [[nodiscard]] Array parse_array();
        [[nodiscard]] Object parse_object();

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
