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
#include "../core/SceneObject.h"

#include "../core/Material.h"
#include "../core/Mesh.h"

namespace holubiho {

    /// @brief Component that defines a 3D model in the scene. Contains a pointer to a Mesh and Material, as well as properties for rendering options like clickability, culling, and whether to draw the model or its UUID. Provides a Draw method to render the mesh using the assigned material and shader.
    class ModelComponent : public Component {
    public:
        Mesh* mesh = nullptr;
        Material* mat = nullptr;
        bool isClickable = false;
        // If true, the model will be rendered with front face culling, which can be used for effects like outlining. When enabled, the Draw method will temporarily set OpenGL to cull front faces before drawing the mesh, and then restore it to cull back faces afterward.
        bool cullFront = false;
        bool drawUUID = true;
        bool draw = true;
        ModelComponent(SceneObject* owner) : Component(owner){}

        void Draw(ShaderProgram* shader) {
            if (!mesh || !shader || !owner || !owner->transform) {
                return;
            }

            static Material fallbackMaterial;
            if (mat) {
                mat->SendToShader(shader);
            } else {
                fallbackMaterial.SendToShader(shader);
            }
            auto modelMatrix = owner->transform->worldMatrix;
            if (cullFront) glCullFace(GL_FRONT);
            mesh->Draw(modelMatrix, shader);
            if (cullFront) glCullFace(GL_BACK);
        }

    };
}



#endif //VIEWER_MESHCOMPONENT_H
