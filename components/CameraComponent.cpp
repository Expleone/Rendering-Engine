//
// Created by expleoene on 3/30/26.
//

#include "CameraComponent.h"

#include "imgui.h"
#include "TransformComponent.h"
#include "../core/SceneObject.h"
#include "../core/InputHandler.h"

namespace holubiho {
    CameraComponent::CameraComponent(SceneObject* owner) : Component(owner) {
        if (auto transform = owner->GetComponent<TransformComponent>()) {
            transform->inheritScale = false;
        }
    }

    glm::mat4 CameraComponent::GetViewMat() {
        if (!this->owner || !this->owner->transform) {
            return glm::mat4(1.0f);
        }
        const glm::mat4& worldMatrix = this->owner->transform->worldMatrix;
        glm::mat3 rotation(worldMatrix);
        glm::mat3 rotationTransposed = glm::transpose(rotation);
        glm::vec3 translation(worldMatrix[3]);
        glm::vec3 invertedTranslation = -(rotationTransposed * translation);
        glm::mat4 viewMatrix(rotationTransposed);
        viewMatrix[3] = glm::vec4(invertedTranslation, 1.0f);

        return viewMatrix;
    }

    glm::mat4 CameraComponent::GetProjectionMat() {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }




}