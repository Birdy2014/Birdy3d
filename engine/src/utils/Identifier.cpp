#include "utils/Identifier.hpp"

#include <cstdint>
#include <fmt/format.h>
#include <random>

namespace Birdy3d::utils {

    Identifier::Identifier(std::string value)
    {
        m_id = std::atol(value.c_str());
    }

    Identifier Identifier::new_random()
    {
        std::mt19937_64 gen(std::random_device{}());
        return Identifier(gen());
    }

    Identifier Identifier::empty()
    {
        return Identifier(0);
    }

    bool Identifier::operator==(Identifier other)
    {
        return m_id == other.m_id;
    }

    Identifier::operator std::string()
    {
        return fmt::format("{:x}", m_id);
    }

}

namespace Birdy3d::serializer {

    template <>
    Value adapter_save(utils::Identifier& value)
    {
        return String{static_cast<std::string>(value)};
    }

    template <>
    void adapter_load(Value* from, utils::Identifier& to)
    {
        if (auto string_ptr = std::get_if<String>(from)) {
            to = utils::Identifier{string_ptr->value};
        }
    }

}
