#pragma once

#include <ranges>
#include <vector>

namespace Birdy3d::utils {

    // TODO: Replace with C++23 std::ranges::to
    template <std::ranges::range R>
    auto to_vector(R&& r)
    {
        auto r_common = r | std::views::common;
        return std::vector(r_common.begin(), r_common.end());
    }

}
