//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_TRANSFORMCOMPONENT_H
#define VIEWER_TRANSFORMCOMPONENT_H
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Component.h"

namespace BiBuild {
    class TransformComponent : public Component {
    public:
        glm::vec3 localPosition{0, 0, 0};
        glm::vec3 localRotation{0, 0, 0};
        glm::vec3 localScale{1.0f, 1.0f, 1.0f};
        glm::mat4 worldMatrix{};

        bool inheritScale = true;
        SceneObject* parent = nullptr;
        std::vector<SceneObject*> children;

        TransformComponent(SceneObject* owner) : Component(owner){}

        [[nodiscard]] glm::mat4 GetLocalMatrix() const {
            glm::mat4 m = glm::translate(glm::mat4(1.0), localPosition);
            m = glm::rotate(m, localRotation.x, glm::vec3(1, 0, 0));
            m = glm::rotate(m, localRotation.y, glm::vec3(0, 1, 0));
            m = glm::rotate(m, localRotation.z, glm::vec3(0, 0, 1));
            m = glm::scale(m, localScale);
            return m;
        }

    };
}



#endif //VIEWER_TRANSFORMCOMPONENT_H
