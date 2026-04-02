//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_VERTEXARRAY_H
#define VIEWER_VERTEXARRAY_H
#include <glad/glad.h>

#include "VertexBuffer.h"

namespace BiBuild {

    class VertexArray {
    public:
        GLuint ID;

        VertexArray();

        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void LinkAttribute(VertexBuffer& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
    };

} // BiBuild

#endif //VIEWER_VERTEXARRAY_H
