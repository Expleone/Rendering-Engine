//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_INDEXBUFFER_H
#define VIEWER_INDEXBUFFER_H
#include <glad/glad.h>
namespace holubiho {

    class IndexBuffer {
    public:
        GLuint ID;
        GLuint count;

        IndexBuffer(const GLuint* data, GLuint count);

        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;
        GLuint GetCount() const;
        void UpdateData(const GLuint* data, GLuint count);

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
    };

} // BiBuild

#endif //VIEWER_INDEXBUFFER_H
