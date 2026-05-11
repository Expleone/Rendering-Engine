//
// Created by expleoene on 4/20/26.
//

#include "SkyboxScript.h"

#include <iostream>

#include "../core/Time.h"


namespace holubiho {
    void SkyboxScript::Update() {
        if (!sunlight) return;
        if (button && button->hasBeenInteracted) {
            isDayAndNightCycleActive = !isDayAndNightCycleActive;
            button->hasBeenInteracted = false; // Reset the interaction state
        }

        if (!isDayAndNightCycleActive) return;


        double coefficient = glm::pi<double>() / dayLengthSeconds * 2.0;

        glm::vec3 curDir = sunlight->direction;

        float currentAngle = glm::atan(curDir.y, curDir.x);
        float newAngle = currentAngle + static_cast<float>(coefficient * holubiho::Time::DeltaTime());

        curDir.x = glm::cos(newAngle);
        curDir.y = glm::sin(newAngle);
        sunlight->direction = glm::normalize(curDir);

        sunPos = -curDir;
    }


} // BiBuild