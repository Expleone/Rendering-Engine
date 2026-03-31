//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_MESHCOMPONENT_H
#define VIEWER_MESHCOMPONENT_H
#include <string>
#include <vector>

#include  "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

namespace BiBuild {
    struct Vertex {
        glm::vec3 position;  // Where the vertex is (X, Y, Z)
        glm::vec3 normal;    // Which way the surface faces (for lighting)
        glm::vec2 texCoords; // Where the texture wraps (U, V)
        glm::vec3 color;     // Vertex-specific colors
    };

    class MeshComponent : public Component {
    public:
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;
        bool isDirty = true;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        // std::string topology = "Triangles";
        MeshComponent(SceneObject* owner) : Component(owner), vertices() {}
        ~MeshComponent() override {
            // Must clean up GPU memory when the component is destroyed
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    };
}



#endif //VIEWER_MESHCOMPONENT_H
