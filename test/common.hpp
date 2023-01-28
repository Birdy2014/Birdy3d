#pragma once

#include "Birdy3d.hpp"
#include <doctest.h>

using namespace Birdy3d;

namespace std {

    std::ostream& operator<<(std::ostream& os, std::u32string const& value);

}

namespace doctest {

    template <>
    struct StringMaker<char32_t> {
        static String convert(char32_t const& value)
        {
            char32_t c[2];
            c[0] = value;
            c[1] = 0;
            return toString(utils::Unicode::utf32_to_utf8(std::u32string(c)));
        }
    };

}
