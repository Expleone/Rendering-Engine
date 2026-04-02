//
// Created by expleoene on 4/1/26.
//

#include "VertexBuffer.h"

namespace BiBuild {
    VertexBuffer::VertexBuffer(const void* data, GLsizeiptr size) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void VertexBuffer::UpdateData(const void* data, GLsizeiptr size) const {
        Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        Unbind();
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &ID);
    }

    void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }
    void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
} // BiBuild