#pragma once

#include "core/ResourceHandle.hpp"
#include "render/Shader.hpp"
#include "ui/Units.hpp"
#include "utils/Color.hpp"
#include <glm/glm.hpp>
#include <limits>
#include <map>
#include <memory>
#include <string>

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace Birdy3d::ui {

    class Theme;

    struct TextVertex {
        glm::vec2 position;
        glm::vec2 texcoords;
        glm::vec4 color;

        TextVertex(glm::ivec2 position, glm::vec2 texcoords, glm::vec4 color)
            : position(position)
            , texcoords(texcoords)
            , color(color)
        { }
    };

    struct Character {
        glm::vec2 texcoord1;
        glm::vec2 texcoord2;
        glm::ivec2 size;
        glm::ivec2 bearing;
        long advance;
    };

    class TextRenderer {
    public:
        TextRenderer(Theme&);
        ~TextRenderer();
        void render_text(std::u32string text, int x, int y, int font_size);
        Size text_size(std::string text, int font_size = 0, std::size_t n = std::numeric_limits<std::size_t>::max());
        Size text_size(std::u32string text, int font_size = 0, std::size_t n = std::numeric_limits<std::size_t>::max());
        float char_width(char32_t c, int font_size = 0);
        static std::size_t text_length(std::u32string);

    private:
        Theme& m_theme;
        std::map<char32_t, Character> m_chars;
        FT_Library* m_ft;
        FT_Face* m_face;
        unsigned int m_font_size;
        GLuint m_texture_atlas;
        glm::vec2 m_texture_atlas_size;
        glm::ivec2 m_texture_atlas_current_pos;
        int m_texture_atlas_current_line_height{0};
        std::vector<TextVertex> m_vertices;
        std::vector<GLuint> m_indices;

        GLuint m_vao, m_vbo, m_ebo;
        core::ResourceHandle<render::Shader> m_shader;

        // Buffer size is smaller than text_length if the text contains escape characters.
        std::size_t m_buffer_char_capacity = 0;
        std::size_t m_escaped_text_length = 0;

        void create_buffers();
        void delete_buffers();

        bool add_char(char32_t c);
        utils::Color::Name parse_color_escape(char32_t);
    };

}
