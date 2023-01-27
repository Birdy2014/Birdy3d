#include "ui/OpenGLPainter.hpp"
#include "core/Application.hpp"
#include "core/ResourceManager.hpp"
#include "events/EventBus.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "ui/Rect.hpp"
#include "ui/Text.hpp"
#include "ui/Theme.hpp"
#include <glad/glad.h>

namespace Birdy3d::ui {

    OpenGLPainter::OpenGLPainter()
    {
        m_color_shader = core::ResourceManager::get_shader("file::ui_rectangle.glsl:TEXTURE=0");
        m_texture_shader = core::ResourceManager::get_shader("file::ui_rectangle.glsl:TEXTURE=1");

        // clang-format off
        float rectangle_vertices[] = {
            0.0f, 0.0f, 0.0, 1.0,
            1.0f, 0.0f, 1.0, 1.0,
            0.0f, 1.0f, 0.0, 0.0,
            1.0f, 1.0f, 1.0, 0.0
        };
        // clang-format on

        // Create buffers
        glGenVertexArrays(1, &m_rectangle_vao);
        glGenBuffers(1, &m_rectangle_vbo);
        // Write to buffers
        glBindVertexArray(m_rectangle_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_rectangle_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0], GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // vertex colors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // clang-format off
        float triangle_vertices[] = {
            0.5,           1.0, 0.0f, 0.0f,
            0.06698729811, 0.25, 0.0f, 1.0f,
            0.9330127019,  0.25, 1.0f, 0.0f,
        };
        // clang-format on

        // Create buffers
        glGenVertexArrays(1, &m_triangle_vao);
        glGenBuffers(1, &m_triangle_vbo);
        // Write to buffers
        glBindVertexArray(m_triangle_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_triangle_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), &triangle_vertices[0], GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // vertex colors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        core::Application::event_bus->subscribe<events::WindowResizeEvent>([this](events::WindowResizeEvent const& event) {
            recreate_projection_matrix(event.width, event.height);
        });

        glm::vec2 viewport = core::Application::get_viewport_size();
        recreate_projection_matrix(viewport.x, viewport.y);
    }

    OpenGLPainter::~OpenGLPainter()
    {
        glDeleteBuffers(1, &m_rectangle_vbo);
        glDeleteVertexArrays(1, &m_rectangle_vao);
    }

    [[nodiscard]] Rect const& OpenGLPainter::visible_rectangle() const
    {
        return m_visible_rectangle;
    }

    void OpenGLPainter::visible_rectangle(Rect rectangle)
    {
        m_visible_rectangle = std::move(rectangle);

        // Transform to OpenGL coordinates
        auto viewport_y = core::Application::get_viewport_size().y;
        auto visible_pos_bottom_moved_origin = viewport_y - m_visible_rectangle.bottom_right().y;

        glScissor(m_visible_rectangle.left(), visible_pos_bottom_moved_origin - 1, m_visible_rectangle.width() + 2, m_visible_rectangle.height() + 2);
    }

    void OpenGLPainter::paint_rectangle_filled(Rect const& rect, utils::Color const& fill_color, unsigned int outline_width, utils::Color const& outline_color) const
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(rect.position(), 0.0f));
        transform = glm::scale(transform, glm::vec3(rect.size(), 1.0f));

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_color_shader->use();
        m_color_shader->set_mat4("projection", m_projection_matrix);
        m_color_shader->set_mat4("transform", transform);
        m_color_shader->set_vec4("fill_color", fill_color);
        m_color_shader->set_vec4("outline_color", outline_color);
        m_color_shader->set_int("outline_width", outline_width);
        glBindVertexArray(m_rectangle_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void OpenGLPainter::paint_rectangle_texture(Rect const& rect, render::Texture const& texture) const
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(rect.position(), 0.0f));
        transform = glm::scale(transform, glm::vec3(rect.size(), 1.0f));

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.id());
        m_texture_shader->use();
        m_texture_shader->set_mat4("projection", m_projection_matrix);
        m_texture_shader->set_mat4("transform", transform);
        m_texture_shader->set_int("rect_texture", 0);
        glBindVertexArray(m_rectangle_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void OpenGLPainter::paint_triangle_filled(Rect const& rect, float orientation, utils::Color const& fill_color) const
    {
        auto transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(rect.position(), 0.0f));
        transform = glm::rotate(transform, orientation, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(rect.size(), 1.0f));

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_color_shader->use();
        m_color_shader->set_mat4("projection", m_projection_matrix);
        m_color_shader->set_mat4("transform", transform);
        m_color_shader->set_vec4("fill_color", fill_color);
        m_color_shader->set_vec4("outline_color", utils::Color::NONE);
        m_color_shader->set_int("outline_width", 0);
        glBindVertexArray(m_triangle_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    }

    void OpenGLPainter::paint_text(glm::ivec2 position, TextDescription const& text) const
    {
        auto start_position_y = position.y;
        auto bottom_position_y = position.y;
        int line_height = text.font_size(); // FIXME: Add line_height() function to TextDescription

        std::u32string output_text;
        auto& lines = text.lines();
        auto it = lines.cbegin();

        // Move to start position
        for (; it != lines.cend(); ++it) {
            bottom_position_y += line_height;

            if (bottom_position_y > m_visible_rectangle.top()) {
                start_position_y = bottom_position_y - line_height;
                break;
            }
        }

        // Add lines
        for (; it != lines.cend(); ++it) {
            bottom_position_y += line_height;

            if (bottom_position_y - line_height > m_visible_rectangle.bottom())
                continue;

            if (it + 1 != lines.cend())
                output_text.append(*it + U'\n');
            else
                output_text.append(*it);
        }

        core::Application::theme().text_renderer().render_text(output_text, position.x, start_position_y, text.font_size());
    }

    void OpenGLPainter::recreate_projection_matrix(int viewport_width, int viewport_height)
    {
        // The -1 is necessary, because the parameter describes the rightmost/top coordinate, not the screen size
        m_projection_matrix = glm::ortho(0.0f, viewport_width - 1.0f, viewport_height - 1.0f, 0.0f);
    }

}
