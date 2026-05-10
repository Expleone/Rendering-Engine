//
// Created by expleoene on 4/28/26.
//

#ifndef VIEWER_FRAMEBUFFER_H
#define VIEWER_FRAMEBUFFER_H
#include <iostream>

#include "glad/glad.h"

namespace holubiho {
    class FrameBuffer {
        GLuint fbo = 0;
        GLuint frameBuffTex = 0;
        GLuint depthRbo = 0;
        GLint internalFormat; // e.g., GL_RGBA32UI or GL_RGBA8
        GLenum dataFormat;    // e.g., GL_RGBA_INTEGER or GL_RGBA
        GLenum dataType;      // e.g., GL_UNSIGNED_INT or GL_UNSIGNED_BYTE
        /// UUID: FrameBuffer uuidFbo(800, 600, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
        /// std: FrameBuffer colorFbo(800, 600, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);


        int width, height;

        void createFrameBuffTex() {
            if (frameBuffTex != 0) glDeleteTextures(1, &frameBuffTex);
            glGenTextures(1, &frameBuffTex);
            glBindTexture(GL_TEXTURE_2D, frameBuffTex);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffTex, 0);
        }

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


        void Bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

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
