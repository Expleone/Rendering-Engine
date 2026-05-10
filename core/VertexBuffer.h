//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_VERTEXBUFFER_H
#define VIEWER_VERTEXBUFFER_H

#include <glad/glad.h>
namespace holubiho {
    class VertexBuffer {
    public:
        GLuint ID{};

        VertexBuffer(const void* data, GLsizeiptr size);

        ~VertexBuffer();
        void Bind() const;
        void Unbind() const;
        void UpdateData(const void* data, GLsizeiptr size) const;

        // Prevent copying
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
    };
}

#endif //VIEWER_VERTEXBUFFER_H
