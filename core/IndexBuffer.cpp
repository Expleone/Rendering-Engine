//
// Created by expleoene on 4/1/26.
//

#include "IndexBuffer.h"

namespace holubiho {
    IndexBuffer::IndexBuffer(const GLuint* data, GLuint count) : count(count) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &ID);
    }

    void IndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
    void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    GLuint IndexBuffer::GetCount() const { return count; }
    void IndexBuffer::UpdateData(const GLuint* data, GLuint count) {
        this->count = count;
        Bind();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(GLuint), data);
        Unbind();
    }
} // BiBuild