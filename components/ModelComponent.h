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

namespace BiBuild {

    class ModelComponent : public Component {
    public:
        Mesh* mesh = nullptr;
        Material* mat = nullptr;
        ModelComponent(SceneObject* owner) : Component(owner){}

        void Draw(ShaderProgram* shader) {
            if (mat) mat->SendToShader(shader);
            auto modelMatrix = owner->transform->worldMatrix;
            mesh->Draw(modelMatrix, shader);
        }

    };
}



#endif //VIEWER_MESHCOMPONENT_H
