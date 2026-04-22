//
// Created by expleoene on 4/22/26.
//

#include "CameraScript.h"

#include "imgui.h"
#include "../core/Time.h"

namespace BiBuild {
    void CameraScript::Update() {
    if (!this->owner || !this->owner->transform || !camera) {
            return;
        }

        const bool canControlMouse = !ImGui::GetIO().WantCaptureMouse;
        const bool isLookPressed = InputManager::IsKeyPressed(GLFW_MOUSE_BUTTON_RIGHT);
        auto* lookState = &camera->lookState;

        if (canControlMouse && isLookPressed) {
            if (!lookState->isMouseCaptured) {
                lookState->isMouseCaptured = true;
                InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                lookState->lastMouseX = InputManager::lastMousePos.x;
                lookState->lastMouseY = InputManager::lastMousePos.y;
            }

            double mouseX = InputManager::currentMousePos.x;
            double mouseY = InputManager::currentMousePos.y;

            const float deltaX = static_cast<float>(mouseX - lookState->lastMouseX);
            const float deltaY = static_cast<float>(mouseY - lookState->lastMouseY);
            lookState->lastMouseX = mouseX;
            lookState->lastMouseY = mouseY;

            lookState->yawDegrees += deltaX * lookState->mouseSensitivity;
            lookState->pitchDegrees -= deltaY * lookState->mouseSensitivity;
            lookState->pitchDegrees = std::clamp(lookState->pitchDegrees, -89.0f, 89.0f);
        } else if (lookState->isMouseCaptured) {
            lookState->isMouseCaptured = false;
            InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        const glm::vec3 forward = CameraComponent::GetForwardFromYawPitch(lookState->yawDegrees, lookState->pitchDegrees);
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

        const float speed = InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ? movementSpeed * 5.0f : movementSpeed;
        this->owner->transform->localPosition += moveDir * speed * static_cast<float>(Time::DeltaTime());
    }
}
