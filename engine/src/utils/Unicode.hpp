#pragma once

#include <string>

namespace Birdy3d::utils {

    class Unicode {
    public:
        static std::u32string utf8_to_utf32(const std::string&);
        static std::string utf32_to_utf8(const std::u32string&);
    };

}
