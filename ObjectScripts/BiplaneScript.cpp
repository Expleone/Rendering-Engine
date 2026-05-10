//
// Created by expleoene on 5/7/26.
//

#include "BiplaneScript.h"

#include "../core/Time.h"
#include "glm/vec3.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "../components/ModelComponent.h"

namespace BiBuild {


    void BiplaneScript::Update() {


        if (airscrew) {
            airscrew->transform->Rotate(glm::angleAxis(glm::radians(360.0f * static_cast<float>(Time::DeltaTime() * 2)), glm::vec3(0, 1, 0)));
        }
        if (debugPtr && *debugPtr && isOnFlightPass) UpdateFlightPath();
        if (debugPtr) {
            if (*debugPtr != prevDebugValue) {
                for (auto cpObj : controlPointObjects) {
                    cpObj->render = (*debugPtr);
                    if (cpObj->GetComponent<ModelComponent>())
                    cpObj->GetComponent<ModelComponent>()->drawUUID = (*debugPtr);
                }
                prevDebugValue = *debugPtr;
            }
        }
        if (plane && plane->hasBeenInteracted && !isMoving) {
            isMoving = true;
            createCurveToLocalPoint(flightPath->getPoint(0.8), flightPath->getPoint(0.81) - flightPath->getPoint(0.8));
            moveTime = 5.0f;
        }
        if (isMoving) Move();
        // owner->transform->localPosition += owner->transform->Forward() * 0.5f * static_cast<float>(Time::DeltaTime());
    }

    void BiplaneScript::Move() {
        curveParam += static_cast<float>(Time::DeltaTime() / moveTime);
        if (curveParam > 1.0f) {
            if (!isOnFlightPass) {
                isOnFlightPass = true;
                UpdateFlightPath();
                curveParam = 0.8f;
                moveTime = 20.0f;
            } else
            curveParam = curveParam - static_cast<int>(curveParam);
        }
        const auto pos = owner->transform->localPosition;
        const auto nextPos = flightPath ? flightPath->getPoint(curveParam) : pos;
        glm::vec3 moveDir = nextPos - pos;

        if (glm::length(moveDir) > 0.0001f) {
            glm::vec3 dir = glm::normalize(moveDir);

            glm::vec3 rightVector = owner->transform->localRotation * glm::vec3(1.0f, 0.0f, 0.0f);

            float turnRate = glm::dot(rightVector, dir);

            LookInDirection(moveDir);

            float bankSensitivity = 10.0f; // Adjust to control how easily it banks
            float maxBankAngle = glm::radians(60.0f); // Maximum roll angle
            float bankAngle = glm::clamp(turnRate * bankSensitivity, -maxBankAngle, maxBankAngle);

            owner->transform->Rotate(glm::angleAxis(-bankAngle, glm::vec3(0, 0, 1)), TransformComponent::TransformSpace::Local);
        }
        owner->transform->localPosition = nextPos;

    }

    void BiplaneScript::UpdateFlightPath() {
        if (controlPointObjects.empty()) return;
        std::vector<glm::vec3> controlPoints;
        for (auto cpObj : controlPointObjects) {
            controlPoints.push_back(cpObj->transform->localPosition);
        }
        if (flightPath) {
            flightPath.reset();
        }
        this->flightPath = std::make_unique<Curve>(Curve::createClosedLoop(controlPoints));
    }

    void BiplaneScript::LookInDirection(glm::vec3 targetDirection) {
        if (glm::length(targetDirection) < 0.0001f) return;
        glm::vec3 dir = glm::normalize(targetDirection);

        glm::vec3 up = glm::vec3(0,1,0);

        if (glm::abs(glm::dot(dir, up)) > 0.999f) {
            glm::vec3 right = owner->transform->localRotation * glm::vec3(1.0f, 0.0f, 0.0f);
            up = glm::normalize(glm::cross(right, dir));
        }

        owner->transform->localRotation = glm::quatLookAt(dir, up);
    }


    void BiplaneScript::createCurveToLocalPoint(const glm::vec3 &point, const glm::vec3& dir) {
        auto A = owner->transform->localPosition;
        auto B = point;

        float distance = glm::length(B - A);

        auto C1 = A + (owner->transform->Forward()*(distance*1.25f));
        auto C2 = B - (glm::normalize(dir) * (distance * 0.25f));
        std::vector<glm::vec3>controlPoints{A,C1,C2,B};
        flightPath = std::make_unique<Curve>(controlPoints);
    }
} // BiBuild