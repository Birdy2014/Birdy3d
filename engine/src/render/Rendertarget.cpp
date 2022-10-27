#include "render/Rendertarget.hpp"

namespace Birdy3d::render {

    std::shared_ptr<Rendertarget> Rendertarget::DEFAULT;

    Rendertarget::Rendertarget()
        : m_width(1)
        , m_height(1)
    { }

    Rendertarget::Rendertarget(int width, int height)
        : m_width(width)
        , m_height(height)
    { }

    Rendertarget::Rendertarget(int width, int height, std::initializer_list<Texture::Preset> presets, bool depth_rbo)
        : m_width(width)
        , m_height(height)
    {
        for (Texture::Preset preset : presets)
            add_texture(preset);
        if (depth_rbo)
            add_depth_rbo();
        finish();
    }

    Rendertarget::Rendertarget(int width, int height, GLuint id)
        : m_initialized(true)
        , m_id(id)
        , m_width(width)
        , m_height(height)
    { }

    Rendertarget::~Rendertarget()
    {
        glDeleteFramebuffers(1, &m_id);
        if (m_rbo_depth != 0)
            glDeleteRenderbuffers(1, &m_rbo_depth);
    }

    Texture* Rendertarget::add_texture(Texture::Preset preset)
    {
        m_initialized = false;
        auto texture = std::make_unique<Texture>(m_width, m_height, preset);
        auto ptr = texture.get();
        m_textures.push_back(std::move(texture));
        return ptr;
    }

    void Rendertarget::add_depth_rbo()
    {
        if (m_rbo_depth != 0)
            return;
        m_initialized = false;
        glGenRenderbuffers(1, &m_rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    }

    bool Rendertarget::finish()
    {
        std::vector<GLenum> attachments;

        glGenFramebuffers(1, &m_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);

        std::size_t color_attachment_id = 0;
        for (auto const& texture : m_textures) {
            if (texture->is_depth()) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id(), 0);
                continue;
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachment_id, GL_TEXTURE_2D, texture->id(), 0);
            attachments.push_back(GL_COLOR_ATTACHMENT0 + color_attachment_id);
            ++color_attachment_id;
        }

        glDrawBuffers(attachments.size(), attachments.data());

        if (m_rbo_depth != 0)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_depth);

        m_initialized = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        return m_initialized;
    }

    void Rendertarget::resize(int width, int height)
    {
        m_width = width;
        m_height = height;

        if (!m_initialized)
            return;

        for (auto const& texture : m_textures)
            texture->resize(m_width, m_height);

        if (m_rbo_depth == 0)
            return;

        glDeleteFramebuffers(1, &m_id);
        glDeleteRenderbuffers(1, &m_rbo_depth);
        m_rbo_depth = 0;
        add_depth_rbo();
        finish();
    }

    void Rendertarget::bind()
    {
        if (!m_initialized) {
            core::Logger::critical("Trying to bind uninitialized Framebuffer");
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
        glViewport(0, 0, m_width, m_height);
    }

}
