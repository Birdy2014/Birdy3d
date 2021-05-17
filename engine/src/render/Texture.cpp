#include "render/Texture.hpp"

#include "core/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Birdy3d {

    Texture::Texture(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat, GLenum pixelType) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Texture::Texture(const std::string& filePath, const std::string& type, const std::string& name)
        : type(type)
        , name(name) {
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
                Logger::error("Invalid number of texture channels: " + nrChannels);

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
            Logger::warn("Failed to load texture at: " + filePath);
        }
        stbi_image_free(data);
    }

    // TODO: test this function and use it for Models without texture and solid color
    Texture::Texture(unsigned char r, unsigned char g, unsigned char b, unsigned char a, const std::string& type) {
        this->type = type;
        unsigned char data[4] = { r, g, b, a };
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &id);
    }

}
