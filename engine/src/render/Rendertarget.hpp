#pragma once

#include "core/Base.hpp"
#include "render/Texture.hpp"

namespace Birdy3d::render {

    class Rendertarget {
    public:
        static std::shared_ptr<Rendertarget> DEFAULT;

        Rendertarget();
        Rendertarget(int width, int height);
        Rendertarget(int width, int height, std::initializer_list<Texture::Preset> presets, bool depth_rbo);
        Rendertarget(Rendertarget&) = delete;
        ~Rendertarget();
        Texture* add_texture(Texture::Preset);
        void add_depth_rbo();
        bool finish();
        void resize(int width, int height);
        Texture* get_texture(int index) { return m_textures[index].get(); }
        bool initialized() { return m_initialized; }
        int width() { return m_width; }
        int height() { return m_height; }
        void bind();
        GLuint id() { return m_id; }

    private:
        friend class core::Application;

        bool m_initialized = false;
        GLuint m_id;
        int m_width, m_height;
        std::vector<std::unique_ptr<Texture>> m_textures;
        GLuint m_rbo_depth = 0;

        Rendertarget(int width, int height, GLuint id);
    };

}
