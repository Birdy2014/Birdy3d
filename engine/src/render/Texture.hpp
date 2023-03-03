#pragma once

#include "core/Base.hpp"
#include "utils/Color.hpp"
#include "utils/TextureLoader.hpp"

namespace Birdy3d::render {

    class Texture {
    public:
        enum class Preset {
            NONE,
            COLOR_RGB,
            COLOR_RGBA,
            COLOR_R_FLOAT,
            COLOR_RGB_FLOAT,
            COLOR_RGBA_FLOAT,
            DEPTH
        };

        Texture(utils::TextureLoader::Image const&);
        Texture(utils::Color const&);
        Texture(int width, int height, Preset);
        Texture(Texture&) = delete;
        ~Texture();
        [[nodiscard]] bool transparent() const;
        void bind(int texture_unit) const;
        void resize(int width, int height);
        [[nodiscard]] GLuint id() const;
        [[nodiscard]] bool is_depth() const;
        [[nodiscard]] Preset preset() const { return m_preset; }

    private:
        Preset m_preset = Preset::NONE;
        int m_width, m_height, m_channels;
        bool m_transparent;
        GLuint m_id;
        bool m_depth = false;
        GLenum m_internal_format;
        GLenum m_format;
        GLenum m_type;
        bool m_resizable = false;
    };

}
