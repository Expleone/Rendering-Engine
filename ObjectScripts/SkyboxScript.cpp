//
// Created by expleoene on 4/20/26.
//

#include "SkyboxScript.h"

#include <iostream>

#include "../core/Time.h"


namespace holubiho {
    void SkyboxScript::Update() {
        if (!sunlight) return;


        glm::vec3 curDir = sunlight->direction;

        float currentAngle = glm::atan(curDir.y, curDir.x);
        float newAngle = currentAngle + static_cast<float>(coefficient * Time::DeltaTime());

        curDir.x = glm::cos(newAngle);
        curDir.y = glm::sin(newAngle);
        sunlight->direction = glm::normalize(curDir);

        prevTime = Time::DeltaTime();
        sunPos = -curDir;
    }
} // BiBuild