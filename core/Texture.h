//
// Created by expleoene on 4/13/26.
//

#ifndef VIEWER_TEXTURE_H
#define VIEWER_TEXTURE_H
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "glad/glad.h"
#include "stb_image.h"

namespace BiBuild {

    enum class TexType {
        Tex2D,
        CubeMap
    };


class Texture {
    GLuint texId = 0;
    TexType type = TexType::Tex2D;
    bool mipmap = false;
public:
    Texture(const std::string& filepath, bool mipmap) : texId(0), type(TexType::Tex2D), mipmap(mipmap) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
            if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture at path: " << filepath << std::endl;

            glDeleteTextures(1, &texId);
            texId = 0;

        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture(const std::string& filepath, bool mipmap, GLint wrapMode, GLint filterMode) : texId(0), type(TexType::Tex2D), mipmap(mipmap) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
            if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture at path: " << filepath << std::endl;

            glDeleteTextures(1, &texId);
            texId = 0;

        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture(const std::vector<std::string>& faces, const bool mipmap) : texId(0), type(TexType::CubeMap), mipmap(mipmap) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

        for (int i = 0; i < faces.size(); i++) {
            auto& face = faces[i];
            int width, height, nrChannels;
            unsigned char* data = stbi_load(face.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = GL_RGB;
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);

            } else {
                std::cout << "Failed to load texture at path: " << face << std::endl;

                glDeleteTextures(1, &texId);
                texId = 0;
                break;
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    Texture(TexType texType, int width, int height, bool mipmap, GLuint format) : texId(0), type(texType), mipmap(mipmap) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
        int channels = 1;
        if (format == GL_RGB) channels = 3;
        else if (format == GL_RGBA) channels = 4;

        std::vector<unsigned char> emptyData(width * height * channels, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, emptyData.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void UpdateTexture(void* data, int width, int height, GLuint format) const {
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void UpdateTexture(void* data, int xoffset, int yoffset, int width, int height, GLuint format) const {
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height,format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    ~Texture() {
        if (texId != 0) {
            glDeleteTextures(1, &texId);
        }
    }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    [[nodiscard]] GLuint GetID() const { return texId; }
    [[nodiscard]] const unsigned int *GetIDptr() const { return &texId; }
    [[nodiscard]] TexType GetType() const { return type; }
};

} // BiBuild

#endif //VIEWER_TEXTURE_H
