#include "render/Texture.hpp"

#include "core/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Birdy3d::render {

    Texture::Texture(const std::string& filePath) {
        unsigned char* data = stbi_load(filePath.data(), &this->m_width, &this->m_height, &this->m_channels, 0);

        if (!data) {
            core::Logger::warn("Failed to load texture at: ", filePath);
            return;
        }

        GLenum format = GL_RED;
        switch (m_channels) {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            core::Logger::critical("Invalid number of texture channels: ", m_channels);
            stbi_image_free(data);
            return;
        }

        m_transparent = format == GL_RGBA;

        glGenTextures(1, &m_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    Texture::Texture(const utils::Color& color) {
        glm::vec4 vec = color.value;
        m_transparent = vec.a < 1;
        float data[4] = { vec.r, vec.g, vec.b, vec.a };
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

    Texture::~Texture() {
        glDeleteTextures(1, &m_id);
    }

    bool Texture::transparent() const {
        return m_transparent;
    }

    void Texture::bind(int texture_unit) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

}
