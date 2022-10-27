#pragma once

#include <string>

namespace Birdy3d::utils {

    class Unicode {
    public:
        static std::u32string utf8_to_utf32(std::string const&);
        static std::string utf32_to_utf8(std::u32string const&);
    };

}
