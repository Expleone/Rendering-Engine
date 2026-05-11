#ifndef VIEWER_CAMERASCRIPT_H
#define VIEWER_CAMERASCRIPT_H

#include <iostream>
#include <vector>
#include <utility>
#include <memory>

#include "ObjectScript.h"
#include "../components/CameraComponent.h"
#include "../core/Curve.h"
#include "../core/SceneObject.h"
#include "../core/InputHandler.h"
#include "../components/ModelComponent.h"

namespace holubiho {
    class CameraScript : public ObjectScript {
        CameraComponent* camera = nullptr;
        float movementSpeed = 10.0f;
        std::unique_ptr<Curve> moveAlong;
        bool isMovingAlongCurve = false;
        bool isMovementDisabled = false;
        float curveParam = 0.0f;
        double moveTime = 0.0;
        std::vector<std::pair<glm::vec3, glm::vec3>> points{};
        bool isPointChanged = false;
        int currentPoint = -1;
        SceneObject* tv = nullptr;
        SceneObject* plane = nullptr;
        std::vector<SceneObject*> controlPointObjects;
        std::unique_ptr<Curve> flightPath = nullptr;
        bool prevDebugValue = true;
        bool *debugPtr = nullptr;
        bool onFlightPass = false;

    public:
        CameraScript(SceneObject* owner) : ObjectScript(owner) {
            camera = owner->GetComponent<CameraComponent>();
            if (!camera) {
                std::cout << "Warning: Owner doesn't have CameraComponent\n";
            }
            points.emplace_back(glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(0, 0, -1));
            points.emplace_back(glm::vec3(150.0f, 0.0f, 0.0f), glm::vec3(-1.5, 0, 0));
            points.emplace_back(glm::vec3(300.0f, 100.0f, 0.0f), glm::vec3(-3, -1, 0));
        }

        void Update() override;

        void createCurveToLocalPoint(const glm::vec3 &point, const glm::vec3& dir);
        void MoveAlongCurve(std::unique_ptr<Curve> curve, double time);
        void LookInDirection(glm::vec3 targetDirection);

        void UpdatePlanePosition();
        void UpdateFlightPath();

        float* GetCameraSpeedPtr() { return &movementSpeed; }
        void SetTVObject(SceneObject* tv) { this->tv = tv; }
        void SetPlaneObject(SceneObject* obj) { this->plane = obj; }
        void SetControlPointObjects(const std::vector<SceneObject*>& controlPointObjects) {
            this->controlPointObjects = controlPointObjects;
            UpdateFlightPath();
        }
        void SetDebugPointer(bool* debugPtr) {
            this->debugPtr = debugPtr;
            if (this->debugPtr) prevDebugValue = *this->debugPtr;
        }

        bool checkIfInBounds(glm::vec3 pos);
    };
}

#endif //VIEWER_CAMERASCRIPT_H