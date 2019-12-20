#ifndef BIRDY3D_TEXTURE_HPP
#define BIRDY3D_TEXTURE_HPP

#include <glad/glad.h>
#include "../stb_image.h"
#include <iostream>

class Texture {
public:
    unsigned int id;
	std::string type, path;
    int width, height, nrChannels;
    
    Texture(const char *filePath, std::string type, std::string path) {
		this->type = type;
		this->path = path;
	    unsigned char *data = stbi_load(filePath, &this->width, &this->height, &this->nrChannels, 0);

		if (data) {
			GLenum format;
			if (this->nrChannels == 1)
				format = GL_RED;
			else if (this->nrChannels == 3)
				format = GL_RGB;
			else if (this->nrChannels == 4)
				format = GL_RGBA;

			glGenTextures(1, &id);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, id);
	    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	    	glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    } else {
	    	std::cout << "Failed to load texture at: " << filePath << std::endl;
	    }
	    stbi_image_free(data);
    }
};

#endif