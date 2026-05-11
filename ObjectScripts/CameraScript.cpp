#include "CameraScript.h"
#include "imgui.h"
#include "../core/Time.h"
#include <glm/gtc/quaternion.hpp>
#include <cmath>

namespace holubiho {

    void CameraScript::Update() {
        if (!this->owner || !this->owner->transform || !camera) {
            return;
        }

        if (debugPtr && *debugPtr) UpdateFlightPath();
        if (debugPtr) {
            if (*debugPtr != prevDebugValue) {
                for (auto cpObj : controlPointObjects) {
                    cpObj->render = (*debugPtr);
                    if (auto modelComp = cpObj->GetComponent<ModelComponent>()) {
                        modelComp->drawUUID = (*debugPtr);
                    }
                }
                prevDebugValue = *debugPtr;
            }
        }

        const bool canControlMouse = !ImGui::GetIO().WantCaptureMouse;
        const bool isLookPressed = InputManager::IsActionActive("MoveCamera");
        auto* lookState = &camera->lookState;

        if (canControlMouse && isLookPressed) {
            if (!lookState->isMouseCaptured) {
                lookState->isMouseCaptured = true;
                InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                lookState->lastMouseX = InputManager::GetPreviousMousePos().x;
                lookState->lastMouseY = InputManager::GetPreviousMousePos().y;
            }

            auto mousePos = InputManager::GetMousePos();

            const auto deltaX = static_cast<float>(mousePos.x - lookState->lastMouseX);
            const auto deltaY = static_cast<float>(mousePos.y - lookState->lastMouseY);
            lookState->lastMouseX = mousePos.x;
            lookState->lastMouseY = mousePos.y;

            glm::quat yawQuat = glm::angleAxis(glm::radians(-deltaX * lookState->mouseSensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

            glm::vec3 currentForward = owner->transform->Forward();
            float currentPitch = glm::degrees(std::asin(currentForward.y));
            float pitchDelta = -deltaY * lookState->mouseSensitivity;

            if (currentPitch + pitchDelta > 89.0f) pitchDelta = 89.0f - currentPitch;
            if (currentPitch + pitchDelta < -89.0f) pitchDelta = -89.0f - currentPitch;

            glm::quat pitchQuat = glm::angleAxis(glm::radians(pitchDelta), glm::vec3(1.0f, 0.0f, 0.0f));

            owner->transform->Rotate(yawQuat, TransformComponent::TransformSpace::Parent);
            owner->transform->Rotate(pitchQuat, TransformComponent::TransformSpace::Local);

        } else if (lookState->isMouseCaptured) {
            lookState->isMouseCaptured = false;
            InputManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        const glm::vec3 forward = owner->transform->localRotation * glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        const glm::vec3 up(0.0f, 1.0f, 0.0f);

        glm::vec3 moveDir(0.0f);
        if (!isMovementDisabled) {
            if (InputManager::IsActionActive("MoveBackward")) moveDir -= forward;
            if (InputManager::IsActionActive("MoveLeft")) moveDir -= right;
            if (InputManager::IsActionActive("MoveRight")) moveDir += right;
            if (InputManager::IsActionActive("MoveForward")) moveDir += forward;
            if (InputManager::IsActionActive("MoveDown")) moveDir -= up;
            if (InputManager::IsActionActive("MoveUp")) moveDir += up;
        }

        if (InputManager::IsActionActive("StopMovingAlongCurve")) {
            isMovementDisabled = false;
            isMovingAlongCurve = false;
            currentPoint = -1;
            moveAlong.reset();
        }

        if (InputManager::IsActionActive("MoveToNextPoint")) {
            if (!isPointChanged) {
                moveAlong.reset();

                isPointChanged = true;
                currentPoint += 1;
                if (currentPoint >= static_cast<int>(points.size())) currentPoint = -1;
                moveTime = 5.0;

                if (currentPoint < 0) {
                    isMovingAlongCurve = false;
                    isMovementDisabled = false;
                    return;
                }

                if (currentPoint == 0 && tv != nullptr) {
                    auto tvposition = glm::vec3(tv->transform->worldMatrix[3].x, tv->transform->worldMatrix[3].y, tv->transform->worldMatrix[3].z);
                    points[currentPoint].first = (tvposition - tv->transform->Forward() * 3.0f + glm::vec3(0.0f, 0.3f, 0.0f));
                    points[currentPoint].second = tv->transform->Forward() + glm::vec3(10.0f, 0.0f, 0.0f);
                }

                if (currentPoint == 2 && flightPath) {
                    points[currentPoint].first = flightPath->getPoint(0.0f);
                    points[currentPoint].second = glm::vec3(-6, 0, -17) - flightPath->getPoint(0);
                    onFlightPass = false;
                    moveTime = 3.0;
                }

                createCurveToLocalPoint(points[currentPoint].first, points[currentPoint].second);
                curveParam = 0.0f;
                isMovingAlongCurve = true;
                isMovementDisabled = true;
            }
        } else {
            isPointChanged = false;
        }

        UpdatePlanePosition();

        if (currentPoint == 1 && plane) {
            isMovingAlongCurve = false;
            isMovementDisabled = true;

            float followSpeed = 3.0f;
            float dt = static_cast<float>(Time::DeltaTime());
            float lerpFactor = 1.0f - glm::exp(-followSpeed * dt);

            owner->transform->localPosition = glm::mix(
                owner->transform->localPosition,
                points[currentPoint].first,
                lerpFactor
            );

            glm::vec3 targetDir = glm::normalize(points[currentPoint].second);
            glm::vec3 targetUp(0.0f, 1.0f, 0.0f);

            if (glm::abs(glm::dot(targetDir, targetUp)) > 0.999f) {
                glm::vec3 rightVec = owner->transform->localRotation * glm::vec3(1.0f, 0.0f, 0.0f);
                targetUp = glm::normalize(glm::cross(rightVec, targetDir));
            }

            glm::quat targetRot = glm::quatLookAt(targetDir, targetUp);

            owner->transform->localRotation = glm::slerp(
                owner->transform->localRotation,
                targetRot,
                lerpFactor
            );

        } else if (currentPoint == 2 && flightPath && (curveParam >= 1.0f || onFlightPass)) {
            if (curveParam >= 1.0f) {
                onFlightPass = true;
                curveParam = 0.0f;
                moveTime = 20.0;
            }

            curveParam += static_cast<float>(Time::DeltaTime() / moveTime);
            if (curveParam > 1.0f) {
                curveParam = std::fmod(curveParam, 1.0f);
            }

            const auto pos = owner->transform->localPosition;
            const auto nextPos = flightPath ? flightPath->getPoint(curveParam) : pos;
            owner->transform->localPosition = nextPos;
            LookInDirection(glm::vec3(-6, 0, -17) - nextPos);
        }

        if (isMovingAlongCurve) {
            if (!moveAlong || moveTime <= 0.0) {
                isMovingAlongCurve = false;
                isMovementDisabled = false;
            } else {
                curveParam += static_cast<float>(Time::DeltaTime() / moveTime);
                curveParam = glm::clamp(curveParam, 0.0f, 1.0f);

                const auto pos = owner->transform->localPosition;
                const auto nextPos = moveAlong->getPoint(curveParam);
                LookInDirection(nextPos - pos);
                owner->transform->localPosition = nextPos;

                if (curveParam >= 1.0f) {
                    isMovingAlongCurve = false;
                    isMovementDisabled = false;
                }
            }
        }

        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
        }

        const float speed = InputManager::IsActionActive("Sprint") ? movementSpeed * 5.0f : movementSpeed;
        glm::vec3 newPos = this->owner->transform->localPosition + moveDir * speed * static_cast<float>(Time::DeltaTime());

        this->owner->transform->localPosition = checkIfInBounds(newPos) ? newPos : this->owner->transform->localPosition;
    }

    void CameraScript::createCurveToLocalPoint(const glm::vec3 &point, const glm::vec3& dir) {
        auto A = owner->transform->localPosition;
        auto B = point;

        float distance = glm::length(B - A);

        auto C1 = A + (owner->transform->Forward() * (distance * 0.25f));
        auto C2 = B - (glm::normalize(dir) * (distance * 0.25f));
        std::vector<glm::vec3> controlPoints{A, C1, C2, B};
        moveAlong = std::make_unique<Curve>(controlPoints);
    }

    void CameraScript::MoveAlongCurve(std::unique_ptr<Curve> curve, double time) {
        moveAlong = std::move(curve);
        isMovementDisabled = true;
        curveParam = 0.0f;
        moveTime = time;
        isMovingAlongCurve = true;
    }

    void CameraScript::LookInDirection(glm::vec3 targetDirection) {
        if (glm::length(targetDirection) < 0.0001f) return;
        glm::vec3 dir = glm::normalize(targetDirection);
        glm::vec3 up = glm::vec3(0, 1, 0);

        if (glm::abs(glm::dot(dir, up)) > 0.999f) {
            glm::vec3 right = owner->transform->localRotation * glm::vec3(1.0f, 0.0f, 0.0f);
            up = glm::normalize(glm::cross(right, dir));
        }

        owner->transform->localRotation = glm::quatLookAt(dir, up);
    }

    void CameraScript::UpdatePlanePosition() {
        if (!plane || points.size() < 2) return;
        points[1].first = plane->transform->localPosition - plane->transform->Forward() * 5.0f + glm::vec3(0.0f, 1.0f, 0.0f) * 2.0f;
        points[1].second = plane->transform->Forward();
    }

    void CameraScript::UpdateFlightPath() {
        if (controlPointObjects.empty()) return;
        std::vector<glm::vec3> controlPoints;
        for (auto cpObj : controlPointObjects) {
            controlPoints.push_back(cpObj->transform->localPosition);
        }
        this->flightPath = std::make_unique<Curve>(Curve::createClosedLoop(controlPoints));
    }

    bool CameraScript::checkIfInBounds(glm::vec3 pos) {
        if (pos.x < -30.0f || pos.x > 30.0f || pos.y < 0.0f || pos.y > 50.0f || pos.z < -30.0f || pos.z > 30.0f) {
            return false;
        }
        if (tv && tv->transform) {
            if (glm::distance(pos, glm::vec3(tv->transform->worldMatrix[3])) < 1.0f) {
                return false;
            }
        }
        return true;
    }
}