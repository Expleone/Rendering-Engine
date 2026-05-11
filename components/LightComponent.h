//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_LIGHTCOMPONENT_H
#define VIEWER_LIGHTCOMPONENT_H

#include "Component.h"
#include "glm/vec3.hpp"

namespace holubiho {
    enum class LightType : int { Directional = 0, Point = 1,  Spotlight = 2 };

    /// @brief Component that defines a light source in the scene. Contains properties for light type, intensity, color, direction, attenuation, and cutoff for spotlights.
    class LightComponent : public Component {
    public:
        LightType type = LightType::Point;
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // For directional lights and spotlights
        glm::vec3 attenuation = glm::vec3(1.0, 0.045, 0.0075);
        float cutoff = 12.5f;
        LightComponent(SceneObject* owner) : Component(owner){}
    };
}




#endif //VIEWER_LIGHTCOMPONENT_H
