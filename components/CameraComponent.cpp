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

    glm::mat4 CameraComponent::FastCameraInverse(const glm::mat4& worldMatrix) {
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
        return FastCameraInverse(owner->transform->worldMatrix);;
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

    void CameraComponent::UpdateCameraFromInput(float deltaTime) {
        if (!this->owner || !this->owner->transform) {
            return;
        }

        const bool canControlMouse = !ImGui::GetIO().WantCaptureMouse;
        const bool isLookPressed = InputManager::IsKeyPressed(GLFW_MOUSE_BUTTON_RIGHT);

        if (canControlMouse && isLookPressed) {
            if (!lookState.isMouseCaptured) {
                lookState.isMouseCaptured = true;
                InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                lookState.lastMouseX = InputManager::lastMousePos.x;
                lookState.lastMouseY = InputManager::lastMousePos.y;
            }

            double mouseX = InputManager::currentMousePos.x;
            double mouseY = InputManager::currentMousePos.y;

            const float deltaX = static_cast<float>(mouseX - lookState.lastMouseX);
            const float deltaY = static_cast<float>(mouseY - lookState.lastMouseY);
            lookState.lastMouseX = mouseX;
            lookState.lastMouseY = mouseY;

            lookState.yawDegrees += deltaX * lookState.mouseSensitivity;
            lookState.pitchDegrees -= deltaY * lookState.mouseSensitivity;
            lookState.pitchDegrees = std::clamp(lookState.pitchDegrees, -89.0f, 89.0f);
        } else if (lookState.isMouseCaptured) {
            lookState.isMouseCaptured = false;
            InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        const glm::vec3 forward = GetForwardFromYawPitch(lookState.yawDegrees, lookState.pitchDegrees);
        const glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        const glm::vec3 up(0.0f, 1.0f, 0.0f);

        glm::vec3 moveDir(0.0f);
        if (InputManager::IsKeyPressed(GLFW_KEY_S)) moveDir -= forward;
        if (InputManager::IsKeyPressed(GLFW_KEY_A)) moveDir -= right;
        if (InputManager::IsKeyPressed(GLFW_KEY_D)) moveDir += right;
        if (InputManager::IsKeyPressed(GLFW_KEY_W)) moveDir += forward;
        if (InputManager::IsKeyPressed(GLFW_KEY_Q)) moveDir -= up;
        if (InputManager::IsKeyPressed(GLFW_KEY_E)) moveDir += up;

        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
        }

        const float speed = InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 40.0f : 20.0f;
        this->owner->transform->localPosition += moveDir * speed * deltaTime;
    }
}
