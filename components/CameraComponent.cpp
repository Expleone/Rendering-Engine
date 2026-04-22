//
// Created by expleoene on 3/30/26.
//

#include "CameraComponent.h"

#include "imgui.h"
#include "TransformComponent.h"
#include "../core/SceneObject.h"
#include "../core/InputHandler.h"

namespace BiBuild {
    CameraComponent::CameraComponent(SceneObject* owner) : Component(owner) {
        if (auto transform = owner->GetComponent<TransformComponent>()) {
            transform->inheritScale = false;
        }
    }

    glm::mat4 CameraComponent::FastCameraInverse() {
        if (!this->owner || !this->owner->transform) {
            return {1.0f};
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
    glm::mat4 CameraComponent::GetViewMat() {
        return FastCameraInverse();;
    }

    glm::vec3 CameraComponent::GetForwardFromYawPitch(float yawDegrees, float pitchDegrees) {
        const float yaw = glm::radians(yawDegrees);
        const float pitch = glm::radians(pitchDegrees);
        return glm::normalize(glm::vec3(
            cosf(pitch) * cosf(yaw),
            sinf(pitch),
            cosf(pitch) * sinf(yaw)
        ));
    }

    glm::mat4 CameraComponent::BuildCameraViewMatrix() const {
        if (!this->owner || !this->owner->transform) {
            return {1.0f};
        }

        const glm::vec3 position = this->owner->transform->localPosition;
        const glm::vec3 forward = GetForwardFromYawPitch(lookState.yawDegrees, lookState.pitchDegrees);
        return glm::lookAt(position, position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
    }


}
