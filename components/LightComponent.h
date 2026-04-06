//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_LIGHTCOMPONENT_H
#define VIEWER_LIGHTCOMPONENT_H

#include "Component.h"
#include "glm/vec3.hpp"

namespace BiBuild {
    enum class LightType : int { Directional = 0, Point = 1,  Spotlight = 2 };

    class LightComponent : public Component {
    public:
        LightType type = LightType::Point;
        float intensity = 1.0f;
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // For directional lights
        glm::vec3 attenuation = glm::vec3(1.0, 0.045, 0.0075);
        float cutoff = 12.5f; // For spotlights (not implemented yet)
        LightComponent(SceneObject* owner) : Component(owner){}
    };
}




#endif //VIEWER_LIGHTCOMPONENT_H
