#pragma once

#include "utils/serializer/Types.hpp"

namespace Birdy3d::serializer {

    class JsonParser : public Parser {
    public:
        using Parser::Parser;
        std::unique_ptr<Value> parse() override;

    private:
        // Parse Types
        std::unique_ptr<String> parse_string();
        std::unique_ptr<Number> parse_number();
        std::unique_ptr<Bool> parse_bool();
        std::unique_ptr<Null> parse_null();
        std::unique_ptr<Array> parse_array();
        std::unique_ptr<Object> parse_object();

        // Utils
        void forward();
    };

    class MinimalJsonGenerator : public Generator {
    public:
        using Generator::Generator;
        void generate(Value&) override;

    private:
        void generate_string(String&);
        void generate_number(Number&);
        void generate_bool(Bool&);
        void generate_null(Null&);
        void generate_array(Array&);
        void generate_object(Object&);
    };

    class PrettyJsonGenerator : public Generator {
    public:
        PrettyJsonGenerator(std::ostream& stream, std::size_t indent_width = 4)
            : Generator(stream)
            , m_indent_width(indent_width) { }
        void generate(Value&) override;

    private:
        std::size_t m_indent_width = 4;

        void generate_string(String&);
        void generate_number(Number&);
        void generate_bool(Bool&);
        void generate_null(Null&);
        void generate_array(Array&, std::size_t);
        void generate_object(Object&, std::size_t);
    };

}
