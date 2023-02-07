#pragma once

#include "utils/serializer/Adapter.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <random>

namespace Birdy3d::utils {

    class Identifier {
    public:
        Identifier() = delete;
        Identifier(std::string value);
        static Identifier new_random();
        static Identifier empty();

        bool is_empty() { return m_id == 0; }
        bool operator==(Identifier other);
        explicit operator std::string();

    private:
        std::uint64_t m_id;

        Identifier(std::uint64_t id)
            : m_id(id)
        { }
    };

}

namespace Birdy3d::serializer {

    template <>
    Value adapter_save(utils::Identifier& value);

    template <>
    void adapter_load(Value* from, utils::Identifier& to);

}
