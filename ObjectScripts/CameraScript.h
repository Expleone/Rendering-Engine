//
// Created by expleoene on 4/22/26.
//

#ifndef VIEWER_CAMERASCRIPT_H
#define VIEWER_CAMERASCRIPT_H
#include <iostream>

#include "ObjectScript.h"
#include "../components/CameraComponent.h"
#include "../core/SceneObject.h"
#include "../core/InputHandler.h"

namespace BiBuild {
    class CameraScript : public ObjectScript {
        CameraComponent* camera;
        float movementSpeed = 10.0f;

    public:
        CameraScript(SceneObject* owner) : ObjectScript(owner) {
            camera = owner->GetComponent<CameraComponent>();
            if (!camera) {
                std::cout << "Warning: Owner doesn't have CameraComponent";
            }
        }

        void Update() override;

    };
}




#endif //VIEWER_CAMERASCRIPT_H
