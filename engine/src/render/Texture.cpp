#include "render/Texture.hpp"

#include "core/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Birdy3d::render {

    Texture::Texture(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat, GLenum pixelType) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        m_transparent = format == GL_RGBA;
    }

    Texture::Texture(const std::string& filePath) {
        unsigned char* data = stbi_load(filePath.data(), &this->width, &this->height, &this->nrChannels, 0);

        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 2)
                format = GL_RG;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else
                core::Logger::critical("Invalid number of texture channels: ", nrChannels);

            m_transparent = format == GL_RGBA;

            glGenTextures(1, &id);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            core::Logger::warn("Failed to load texture at: ", filePath);
        }
        stbi_image_free(data);
    }

    Texture::Texture(const utils::Color& color) {
        glm::vec4 vec = color.value;
        m_transparent = vec.a < 1;
        float data[4] = { vec.r, vec.g, vec.b, vec.a };
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &id);
    }

    bool Texture::transparent() {
        return m_transparent;
    }

}
