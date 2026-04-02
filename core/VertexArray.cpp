//
// Created by expleoene on 4/1/26.
//

#include "VertexArray.h"

namespace BiBuild {
    VertexArray::VertexArray() {
        glGenVertexArrays(1, &ID);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &ID);
    }

    void VertexArray::Bind() const { glBindVertexArray(ID); }
    void VertexArray::Unbind() const { glBindVertexArray(0); }

    // Links a VBO attribute to the VAO
    void VertexArray::LinkAttribute(VertexBuffer& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
        vbo.Bind();
        glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        vbo.Unbind();
    }
} // BiBuild