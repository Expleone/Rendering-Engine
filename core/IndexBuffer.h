//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_INDEXBUFFER_H
#define VIEWER_INDEXBUFFER_H
#include <glad/glad.h>
namespace holubiho {

    /// @brief Class that encapsulates the creation and management of an OpenGL index buffer object (IBO). Provides methods for binding, unbinding, updating data, and retrieving the count of indices. The constructor initializes the buffer with provided index data and count, while the destructor ensures proper cleanup of OpenGL resources.
    class IndexBuffer {
    public:
        GLuint ID;
        GLuint count;

        IndexBuffer(const GLuint* data, GLuint count);

        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        /// @brief Returns the number of indices stored in the index buffer.
        GLuint GetCount() const;

        /// @brief Updates the index buffer with new data. This method binds the buffer, updates its data store with the provided index data, and then unbinds the buffer. The count of indices is also updated to reflect the new data.
        void UpdateData(const GLuint* data, GLuint count);

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
    };

} // BiBuild

#endif //VIEWER_INDEXBUFFER_H
