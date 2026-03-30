//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_LIGHTCOMPONENT_H
#define VIEWER_LIGHTCOMPONENT_H

#include "Component.h"

namespace BiBuild {
    enum class LightType { Point, Directional };

    class LightComponent : public Component {
    public:
        LightType type = LightType::Point;
        float intensity = 1.0f;
        LightComponent(SceneObject* owner) : Component(owner){}
    };
}




#endif //VIEWER_LIGHTCOMPONENT_H
