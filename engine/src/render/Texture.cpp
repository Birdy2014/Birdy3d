#include "render/Texture.hpp"

#include "core/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Birdy3d::render {

    Texture::Texture(std::string const& file_path)
    {
        unsigned char* data = stbi_load(file_path.data(), &m_width, &m_height, &m_channels, 0);

        if (!data) {
            core::Logger::warn("Failed to load texture at {}", file_path);
            return;
        }

        GLenum format = GL_RED;
        GLenum internal_format = GL_RED;
        switch (m_channels) {
        case 1:
            format = GL_RED;
            internal_format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            internal_format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            internal_format = GL_SRGB;
            break;
        case 4:
            format = GL_RGBA;
            internal_format = GL_SRGB_ALPHA;
            break;
        default:
            core::Logger::critical("Invalid number of texture channels: {}", m_channels);
            stbi_image_free(data);
            return;
        }

        m_transparent = format == GL_RGBA;

        glGenTextures(1, &m_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    Texture::Texture(utils::Color const& color)
    {
        glm::vec4 vec = color.value;
        m_transparent = vec.a < 1;
        float data[4] = {vec.r, vec.g, vec.b, vec.a};
        glGenTextures(1, &m_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Texture::Texture(int width, int height, Preset preset)
        : m_preset(preset)
        , m_width(width)
        , m_height(height)
        , m_resizable(true)
    {
        glGenTextures(1, &m_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        switch (preset) {
        case Preset::NONE:
            core::Logger::error("Invalid Texture preset");
            return;
        case Preset::COLOR_RGB:
            m_channels = 3;
            m_internalformat = GL_RGB;
            m_format = GL_RGB;
            m_type = GL_UNSIGNED_BYTE;
            break;
        case Preset::COLOR_RGBA:
            m_channels = 4;
            m_internalformat = GL_RGBA;
            m_format = GL_RGBA;
            m_type = GL_UNSIGNED_BYTE;
            break;
        case Preset::COLOR_R_FLOAT:
            m_channels = 1;
            m_internalformat = GL_RED;
            m_format = GL_RED;
            m_type = GL_FLOAT;
            break;
        case Preset::COLOR_RGB_FLOAT:
            m_channels = 3;
            m_internalformat = GL_RGB16F;
            m_format = GL_RGB;
            m_type = GL_FLOAT;
            break;
        case Preset::COLOR_RGBA_FLOAT:
            m_channels = 4;
            m_internalformat = GL_RGBA16F;
            m_format = GL_RGBA;
            m_type = GL_FLOAT;
            break;
        case Preset::DEPTH:
            m_channels = 1;
            m_internalformat = GL_DEPTH_COMPONENT;
            m_format = GL_DEPTH_COMPONENT;
            m_type = GL_FLOAT;
            m_depth = true;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, m_width, m_height, 0, m_format, m_type, nullptr);
        if (m_depth)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        // TODO: Parameters: FILTER, WRAP, COMPARE_MODE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_id);
    }

    bool Texture::transparent() const
    {
        return m_transparent;
    }

    void Texture::bind(int texture_unit)
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::resize(int width, int height)
    {
        if (!m_resizable)
            return;
        m_width = width;
        m_height = height;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, m_width, m_height, 0, m_format, m_type, nullptr);
    }

    GLuint Texture::id() const
    {
        return m_id;
    }

    bool Texture::is_depth() const
    {
        return m_depth;
    }

}
