#pragma once

#include "render/Forward.hpp"
#include "ui/Forward.hpp"
#include "utils/Color.hpp"
#include <memory>

namespace Birdy3d::ui {

    class Painter {
    public:
        static Painter& the();

        virtual ~Painter() = default;

        [[nodiscard]] virtual Rect const& visible_rectangle() const = 0;
        virtual void visible_rectangle(Rect) = 0;

        virtual void paint_rectangle_filled(Rect const&, utils::Color const& fill_color, unsigned int outline_width, utils::Color const& outline_color) const = 0;
        virtual void paint_rectangle_texture(Rect const&, render::Texture const&) const = 0;
        virtual void paint_triangle_filled(Rect const&, float orientation, utils::Color const&) const = 0;
        virtual void paint_text(glm::ivec2 position, TextDescription const&) const = 0;

    private:
        static std::unique_ptr<Painter> m_instance;
    };

}
