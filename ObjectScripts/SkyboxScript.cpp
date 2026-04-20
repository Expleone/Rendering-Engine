//
// Created by expleoene on 4/20/26.
//

#include "SkyboxScript.h"

#include <iostream>


namespace BiBuild {
    void SkyboxScript::Update() {
        if (!sunlight) return;

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - prevTime;

        glm::vec3 curDir = sunlight->direction;

        float currentAngle = glm::atan(curDir.y, curDir.x);
        float newAngle = currentAngle + static_cast<float>(coefficient * deltaTime);

        curDir.x = glm::cos(newAngle);
        curDir.y = glm::sin(newAngle);
        sunlight->direction = glm::normalize(curDir);

        prevTime = currentTime;
        sunPos = -curDir;
    }
} // BiBuild