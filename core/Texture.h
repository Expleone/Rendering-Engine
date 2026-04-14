//
// Created by expleoene on 4/13/26.
//

#ifndef VIEWER_TEXTURE_H
#define VIEWER_TEXTURE_H
#include <iostream>
#include <ostream>
#include <string>

#include "glad/glad.h"
#include "stb_image.h"

namespace BiBuild {

class Texture {
    GLuint texId;

public:
    Texture(const std::string filepath, bool mipmap) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture at path: " << filepath << std::endl;

            glDeleteTextures(1, &texId);
            texId = 0;

        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture() {
        if (texId != 0) {
            glDeleteTextures(1, &texId);
        }
    }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    GLuint GetID() const { return texId; }
};

} // BiBuild

#endif //VIEWER_TEXTURE_H
