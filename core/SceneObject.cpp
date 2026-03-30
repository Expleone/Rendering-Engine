//
// Created by expleoene on 3/27/26.
//

#include "SceneObject.h"

#include "../components/TransformComponent.h"

namespace BiBuild {

    bool SceneObject::AddChild(SceneObject* child) {
        if (auto* transform = GetComponent<TransformComponent>()) {
            transform->children.push_back(child);
            return true;
        }
        return false;
    }
} // BiBuild