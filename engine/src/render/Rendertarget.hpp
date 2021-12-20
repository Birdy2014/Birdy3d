#pragma once

#include "core/Base.hpp"
#include "render/Texture.hpp"

namespace Birdy3d::render {

    class Rendertarget {
    public:
        Rendertarget(int width, int height);
        Rendertarget(Rendertarget&) = delete;
        ~Rendertarget();
        Texture* add_texture(Texture::Preset);
        void add_depth_rbo();
        bool finish();
        void resize(int width, int height);
        void bind();
        GLuint id() { return m_id; }

    private:
        GLuint m_id = 0;
        int m_width, m_height;
        std::vector<std::unique_ptr<Texture>> m_textures;
        GLuint m_rbo_depth = 0;
    };

}
