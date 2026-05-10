//
// Created by expleoene on 5/7/26.
//

#ifndef VIEWER_BIPLANESCRIPT_H
#define VIEWER_BIPLANESCRIPT_H
#include "ObjectScript.h"
#include "../core/SceneObject.h"
#include "../core/Curve.h"

namespace holubiho {
    class BiplaneScript : public ObjectScript {
        SceneObject* airscrew = nullptr;
        SceneObject* plane = nullptr;
        std::unique_ptr<Curve> flightPath = nullptr;
        std::vector<holubiho::SceneObject*> controlPointObjects;
        float curveParam = 0.0f;
        float moveTime = 10.0f;
        bool prevDebugValue = true;
        bool *debugPtr = nullptr;
        bool isOnFlightPass = false;
        bool isMoving = false;

public:
        BiplaneScript(SceneObject* owner) : ObjectScript(owner) {}
        void Update() override;

        void Move();

        void UpdateFlightPath();

        void SetAirscrew(SceneObject* airscrew) {
            this->airscrew = airscrew;
        }
        void SetPlane(SceneObject* plane) {
            this->plane = plane;
        }

        void SetDebugPointer(bool* debugPtr) {
            this->debugPtr = debugPtr;
        }



        void SetFlightPath(Curve path) {
            if (flightPath) {
                flightPath.reset();
            }
            this->flightPath = std::make_unique<Curve>(path);
        }

        void SetControlPointObjects(const std::vector<SceneObject*>& controlPointObjects) {
            this->controlPointObjects = controlPointObjects;
        }
        void LookInDirection(glm::vec3 targetDirection);
        void createCurveToLocalPoint(const glm::vec3 &point, const glm::vec3& dir);
    };
}

#endif //VIEWER_BIPLANESCRIPT_H
