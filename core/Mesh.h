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

namespace BiBuild {
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
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        bool isDirty = true; // Indicates if GPU buffers need updating
        VertexBuffer vbo;
        VertexArray vao;
        IndexBuffer ebo;


    public:
        ShaderProgram* shader = nullptr; // Store a reference to the shader for drawing
        BoundingBox bb;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {}, const std::string& name = "Mesh");

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

        void UpdateVertices(const std::vector<Vertex>& newVertices) {
            vertices = newVertices;
            isDirty = true; // Mark as dirty to update GPU buffers on next draw
        }

        void UpdateIndices(const std::vector<unsigned int>& newIndices) {
            indices = newIndices;
            isDirty = true; // Mark as dirty to update GPU buffers on next draw
        }

        void SetName(const std::string& newName) {
            name = newName;
        }

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
