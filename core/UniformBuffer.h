//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_UNIFORMBUFFER_H
#define VIEWER_UNIFORMBUFFER_H
#include <glad/glad.h>
namespace BiBuild {

    class UniformBuffer {
    public:
        GLuint ID;

        // Constructor allocates the memory and assigns the global binding point
        UniformBuffer(GLsizeiptr size, GLuint bindingPoint) {
            glGenBuffers(1, &ID);
            glBindBuffer(GL_UNIFORM_BUFFER, ID);

            // Allocate memory (GL_DYNAMIC_DRAW since UBOs update often)
            glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            // Link this buffer to the specific binding slot
            glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, ID, 0, size);
        }

        ~UniformBuffer() {
            glDeleteBuffers(1, &ID);
        }

        // Update the entire buffer at once
        void UpdateData(const void* data, GLsizeiptr size) {
            glBindBuffer(GL_UNIFORM_BUFFER, ID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        // Update a specific chunk of the buffer (useful for arrays of data)
        void UpdateSubData(const void* data, GLsizeiptr size, GLintptr offset) {
            glBindBuffer(GL_UNIFORM_BUFFER, ID);
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        // Prevent copying
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
    };
} // BiBuild

#endif //VIEWER_UNIFORMBUFFER_H
