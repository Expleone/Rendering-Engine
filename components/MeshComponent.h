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
    class Mesh;
    // struct Vertex {
    //     glm::vec3 position;  // Where the vertex is (X, Y, Z)
    //     glm::vec3 normal;    // Which way the surface faces (for lighting)
    //     glm::vec2 texCoords; // Where the texture wraps (U, V)
    //     glm::vec3 color;     // Vertex-specific colors
    // };

    class MeshComponent : public Component {
    public:
        Mesh* mesh = nullptr;
        std::string meshName;
        MeshComponent(SceneObject* owner) : Component(owner){}

    };
}



#endif //VIEWER_MESHCOMPONENT_H
