//
// Created by expleoene on 3/30/26.
//

#include "CameraComponent.h"

#include "TransformComponent.h"
#include "../core/SceneObject.h"

namespace BiBuild {
    CameraComponent::CameraComponent(SceneObject* owner) : Component(owner) {
        if (auto transform = owner->GetComponent<TransformComponent>()) {
            transform->inheritScale = false;
        }
    }

    glm::mat4 CameraComponent::FastCameraInverse(const glm::mat4& worldMatrix) {
        // 1. Extract the top-left 3x3 rotation matrix and transpose it
        glm::mat3 rotation(worldMatrix);
        glm::mat3 rotationTransposed = glm::transpose(rotation);

        // 2. Extract the translation vector (the 4th column)
        glm::vec3 translation(worldMatrix[3]);

        // 3. Calculate the inverted translation: -R^T * T
        glm::vec3 invertedTranslation = -(rotationTransposed * translation);

        // 4. Construct the final View Matrix
        // Initialize with the transposed rotation (pads the rest with identity values)
        glm::mat4 viewMatrix(rotationTransposed);

        // Replace the 4th column with the new translation
        viewMatrix[3] = glm::vec4(invertedTranslation, 1.0f);

        return viewMatrix;
    }

    glm::mat4 CameraComponent::GetProjectionMat() {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
    glm::mat4 CameraComponent::GetViewMat() {
        return FastCameraInverse(owner->transform->worldMatrix);;
    }
}
