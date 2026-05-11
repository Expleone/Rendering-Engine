//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_MESH_H
#define VIEWER_MESH_H

#include <uuid.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IndexBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

// #include "../components/MeshComponent.h"

namespace holubiho {
    struct Vertex {
        glm::vec3 position;  // Where the vertex is (X, Y, Z)
        glm::vec3 normal;    // Which way the surface faces (for lighting)
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 texCoords; // Where the texture wraps (U, V)
        glm::vec3 color;     // Vertex-specific colors
    };
    struct BoundingBox {
        glm::vec3 min;
        glm::vec3 max;
    };

    class Mesh {
        uuids::uuid id;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        bool isDirty = true; // Indicates if GPU buffers need updating
        VertexBuffer vbo;
        VertexArray vao;
        IndexBuffer ebo;


    public:
        ShaderProgram* shader = nullptr; // Store a reference to the shader for drawing
        BoundingBox bb;
        std::string name;

        /// @brief Constructs a Mesh with the given vertices, indices, and an optional name.
        /// @param vertices A vector of Vertex structures containing the vertex data for the mesh.
        /// @param indices A vector of unsigned integers representing the index data for the mesh.
        /// @param name An optional name for the mesh, which can be used for identification and debugging purposes.
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {}, const std::string& name = "Mesh");

        /// @brief Draws the mesh using the provided model matrix and shader program. If the mesh is marked as dirty, it updates the GPU buffers before drawing.
        /// @param modelMat The model transformation matrix to apply to the mesh when drawing.
        /// @param program The shader program to use for rendering the mesh.
        void Draw(glm::mat4& modelMat, ShaderProgram* program) const {
            vao.Bind();
            if (isDirty) {
                const_cast<Mesh*>(this)->UpdateGPUBuffers(); // Update GPU buffers if dirty
            }
            ebo.Bind();
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMat)));
            program->SetUniformMat4("model", modelMat);
            program->SetUniformMat3("normalMatrix", normalMatrix);

            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo.GetCount()), GL_UNSIGNED_INT, nullptr);
        }

        /// @brief Updates the vertex data of the mesh with new vertices and marks the mesh as dirty to indicate that GPU buffers need to be updated before the next draw call.
        /// @param newVertices A vector of Vertex structures containing the new vertex data for the mesh
        void UpdateVertices(const std::vector<Vertex>& newVertices) {
            vertices = newVertices;
            isDirty = true; // Mark as dirty to update GPU buffers on next draw
        }

        /// @brief Updates the index data of the mesh with new indices and marks the mesh as dirty to indicate that GPU buffers need to be updated before the next draw call.
        /// @param newIndices A vector of unsigned integers representing the new index data for the mesh
        void UpdateIndices(const std::vector<unsigned int>& newIndices) {
            indices = newIndices;
            isDirty = true; // Mark as dirty to update GPU buffers on next draw
        }

        /// @brief Sets the name of the mesh, which can be used for identification and debugging purposes.
        /// @param newName The new name to assign to the mesh.
        void SetName(const std::string& newName) {
            name = newName;
        }
    private:
        /// @brief Updates the GPU buffers (vertex buffer and index buffer) with the current vertex and index data. This method is called when the mesh is marked as dirty, indicating that the vertex or index data has changed and needs to be re-uploaded to the GPU before the next draw call.
        void UpdateGPUBuffers() {

            // Update Vertex Buffer
            vbo.UpdateData(vertices.data(), static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)));

            // Update Index Buffer
            ebo.UpdateData(indices.data(), static_cast<GLsizeiptr>(indices.size()));

            isDirty = false; // Reset dirty flag after updating
        }

    };
}




#endif //VIEWER_MESH_H
