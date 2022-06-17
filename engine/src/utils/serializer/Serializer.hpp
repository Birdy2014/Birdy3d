#pragma once

#include "core/Logger.hpp"
#include "utils/serializer/Adapter.hpp"
#include "utils/serializer/Json.hpp"
#include <sstream>

namespace Birdy3d::serializer {

    enum class GeneratorType {
        JSON_PRETTY,
        JSON_MINIMAL,
    };

    class Serializer {
    public:
        static void serialize(GeneratorType type, std::string name, auto& value, std::ostream& stream) {
            serializer::Object object;
            serializer::Adapter adapter(&object, Adapter::Mode::SAVE);
            adapter(name, value);
            std::unique_ptr<Generator> generator;
            switch (type) {
            case GeneratorType::JSON_PRETTY:
                generator = std::make_unique<PrettyJsonGenerator>(stream);
                break;
            case GeneratorType::JSON_MINIMAL:
                generator = std::make_unique<MinimalJsonGenerator>(stream);
                break;
            }
            generator->generate(object);
            PointerRegistry::clear();
        }

        static std::string serialize(GeneratorType type, std::string name, auto& value) {
            std::stringstream stream;
            serialize(type, name, value, stream);
            return stream.str();
        }

        static void serialize(GeneratorType type, auto& value, std::ostream& stream) {
            auto object = adapter_save(value);
            std::unique_ptr<Generator> generator;
            switch (type) {
            case GeneratorType::JSON_PRETTY:
                generator = std::make_unique<PrettyJsonGenerator>(stream);
            case GeneratorType::JSON_MINIMAL:
                generator = std::make_unique<MinimalJsonGenerator>(stream);
            }
            generator->generate(object);
            PointerRegistry::clear();
        }

        static std::string serialize(GeneratorType type, auto& value) {
            std::stringstream stream;
            serialize(type, value, stream);
            return stream.str();
        }

        static void deserialize(std::string text, std::string name, auto& value) {
            try {
                serializer::JsonParser parser(text);
                auto parsed = parser.parse();
                auto object = std::get_if<Object>(&parsed);
                if (!object) {
                    core::Logger::critical("Invalid Object");
                    exit(1);
                }
                serializer::Adapter adapter(object, Adapter::Mode::LOAD);
                adapter(name, value);
                serializer::PointerRegistry::clear();
            } catch (ParseError& error) {
                core::Logger::critical("{}", error.what());
            }
        }

        static void deserialize(std::string text, auto& value) {
            try {
                serializer::JsonParser parser(text);
                auto parsed = parser.parse();
                adapter_load(&parsed, value);
                serializer::PointerRegistry::clear();
            } catch (ParseError& error) {
                core::Logger::critical("{}", error.what());
            }
        }
    };

}
