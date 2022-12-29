#pragma once

#include "core/ResourceHandle.hpp"
#include "ui/Painter.hpp"
#include "ui/Rect.hpp"

namespace Birdy3d::ui {

    class OpenGLPainter : public Painter {
    public:
        OpenGLPainter();
        ~OpenGLPainter();

        [[nodiscard]] virtual Rect const& visible_rectangle() const override;
        virtual void visible_rectangle(Rect) override;

        virtual void paint_rectangle_filled(Rect const&, utils::Color const& fill_color, unsigned int outline_width, utils::Color const& outline_color) const override;
        virtual void paint_rectangle_texture(Rect const&, render::Texture const&) const override;
        virtual void paint_triangle_filled(Rect const&, float orientation, utils::Color const&) const override;
        virtual void paint_text(glm::ivec2 position, TextDescription const& text) const override;

    private:
        Rect m_visible_rectangle;
        core::ResourceHandle<render::Shader> m_color_shader, m_texture_shader;
        GLuint m_rectangle_vao, m_rectangle_vbo;
        GLuint m_triangle_vao, m_triangle_vbo;
        glm::mat4 m_projection_matrix;

        void recreate_projection_matrix(int viewport_width, int viewport_height);
    };

}
