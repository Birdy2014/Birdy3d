#include "common.hpp"

TEST_CASE("AbsoluteLayout")
{
    auto container = ui::Container{ui::Container::Options{.size = ui::Size{100_pc}}};
    container.set_layout<ui::AbsoluteLayout>();
    auto inner_widget = container.add_child<ui::Widget>(ui::Widget::Options{.position = ui::Position{-12_pc, 2_pc}, .size{40_px, 53_px}, .placement = ui::Placement::TOP_RIGHT});

    SUBCASE("arrange")
    {
        container.do_layout(ui::Rect::from_position_and_size({0, 0}, {100, 100}));
        CHECK_EQ(inner_widget->absolute_rect().position(), glm::ivec2{48, 2});
        CHECK_EQ(inner_widget->absolute_rect().size(), glm::ivec2{40, 53});
    }
}
