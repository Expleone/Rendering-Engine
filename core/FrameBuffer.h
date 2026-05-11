//
// Created by expleoene on 4/28/26.
//

#ifndef VIEWER_FRAMEBUFFER_H
#define VIEWER_FRAMEBUFFER_H
#include <iostream>

#include "glad/glad.h"

namespace holubiho {

    // @brief Class that encapsulates the creation and management of an OpenGL framebuffer object (FBO) with a color texture attachment and a depth renderbuffer. Provides methods for binding, unbinding, resizing, and retrieving the texture ID of the framebuffer.
    class FrameBuffer {
        GLuint fbo = 0;
        GLuint frameBuffTex = 0;
        GLuint depthRbo = 0;
        GLint internalFormat; // e.g., GL_RGBA32UI or GL_RGBA8
        GLenum dataFormat;    // e.g., GL_RGBA_INTEGER or GL_RGBA
        GLenum dataType;      // e.g., GL_UNSIGNED_INT or GL_UNSIGNED_BYTE


        int width, height;


        /// @brief Creates or recreates the color texture attachment for the framebuffer.
        void createFrameBuffTex() {
            if (frameBuffTex != 0) glDeleteTextures(1, &frameBuffTex);
            glGenTextures(1, &frameBuffTex);
            glBindTexture(GL_TEXTURE_2D, frameBuffTex);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffTex, 0);
        }

        /// @brief Creates or recreates the depth renderbuffer attachment for the framebuffer.
        void createRenderBuff() {
            if (depthRbo != 0) {
                glDeleteRenderbuffers(1, &depthRbo);
            }
            glGenRenderbuffers(1, &depthRbo);
            glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);
        }


    public:

        /// @brief Constructs a FrameBuffer object with the specified width, height, internal format, data format, and data type. Initializes the framebuffer by creating the color texture and depth renderbuffer attachments, and checks for completeness.
        /// @param sWidth The width of the framebuffer in pixels.
        /// @param sHeight The height of the framebuffer in pixels.
        /// @param sInternalFormat The internal format of the color texture (e.g., GL_RGBA32UI for unsigned integer data or GL_RGBA8 for unsigned byte data).
        /// @param sDataFormat The format of the pixel data (e.g., GL_RGBA_INTEGER for unsigned integer data or GL_RGBA for unsigned byte data).
        /// @param sDataType The data type of the pixel data (e.g., GL_UNSIGNED_INT for unsigned integer data or GL_UNSIGNED_BYTE for unsigned byte data).
        FrameBuffer(int sWidth, int sHeight, GLint sInternalFormat, GLenum sDataFormat, GLenum sDataType) {
            width = sWidth;
            height = sHeight;
            internalFormat = sInternalFormat;
            dataFormat = sDataFormat;
            dataType = sDataType;

            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            createFrameBuffTex();
            createRenderBuff();

            GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, drawBuffers);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Warning: Framebuffer is incomplete" << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        ~FrameBuffer() {
            if (depthRbo != 0) {
                glDeleteRenderbuffers(1, &depthRbo);
            }
            if (frameBuffTex != 0) {
                glDeleteTextures(1, &frameBuffTex);
            }
            if (fbo != 0) {
                glDeleteFramebuffers(1, &fbo);
            }
        }

        /// @brief Binds the framebuffer for rendering. Subsequent rendering commands will target this framebuffer until it is unbound.
        void Bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        }

        /// @brief Unbinds the framebuffer, reverting to the default framebuffer (usually the screen). After calling this method, rendering commands will target the default framebuffer.
        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        /// @brief Resizes the framebuffer to the specified width and height. This involves updating the dimensions of the color texture and depth renderbuffer attachments to match the new size.
        /// @param newWidth The new width of the framebuffer in pixels.
        /// @param newHeight The new height of the framebuffer in pixels.
        void OnResize(int newWidth, int newHeight) {
            width = newWidth;
            height = newHeight;
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            createFrameBuffTex();
            createRenderBuff();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint GetTexture() const {
            return frameBuffTex;
        }

    };
} // BiBuild

#endif //VIEWER_FRAMEBUFFER_H
