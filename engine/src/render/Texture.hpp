#pragma once

#include <glad/glad.h>
#include <string>

namespace Birdy3d {

    class Texture {
    public:
        unsigned int id;
        std::string type, path;
        int width, height, nrChannels;

        Texture(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat, GLenum pixelType);
        Texture(const std::string& filePath, const std::string& type, const std::string& path);
        Texture(unsigned char r, unsigned char g, unsigned char b, unsigned char a, const std::string& type);
        ~Texture();
    };

}
