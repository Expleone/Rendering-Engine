//
// Created by expleoene on 4/2/26.
//

#ifndef VIEWER_MATERIAL_H
#define VIEWER_MATERIAL_H
#include "Component.h"

namespace BiBuild {
    class ShaderProgram;

    class MaterialComponent : public Component {
    public:
        MaterialComponent(SceneObject* owner) : Component(owner) {}
        ShaderProgram* shader = nullptr;
    };

} // BiBuild

#endif //VIEWER_MATERIAL_H
