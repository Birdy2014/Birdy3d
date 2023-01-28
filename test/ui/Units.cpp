#include "common.hpp"

TEST_CASE("Dimension") {
    SUBCASE("px") {
        auto dimension = 4_px;
        CHECK_EQ(dimension.to_pixels(), 4);
    }
    SUBCASE("em") {
        auto dimension = 4_em;
        CHECK_EQ(dimension.to_pixels(), 4 * core::Application::theme().line_height());
    }
    SUBCASE("percent") {
        auto dimension = 4_pc;
        CHECK_EQ(dimension.to_pixels(), 0);
        CHECK_EQ(dimension.to_pixels(100), 4);
    }
}
