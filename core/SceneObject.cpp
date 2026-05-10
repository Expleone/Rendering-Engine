//
// Created by expleoene on 3/27/26.
//

#include "SceneObject.h"

#include "../components/TransformComponent.h"

namespace BiBuild {

    bool SceneObject::AddChild(SceneObject* child) {
        if (!child || child == this) return false;

        if (auto* transform = GetComponent<TransformComponent>()) {

            if (child->transform && child->transform->parent) {
                child->transform->parent->DeleteChild(child);
            }

            transform->children.push_back(child);

            if (child->transform) {
                child->transform->parent = this;
            }

            return true;
        }
        return false;
    }

    bool SceneObject::DeleteChild(SceneObject *child) {
        if (!child) return false;

        if (auto* transform = GetComponent<TransformComponent>()) {
            auto it = std::find(transform->children.begin(), transform->children.end(), child);

            if (it != transform->children.end()) {
                transform->children.erase(it);

                if (child->transform && child->transform->parent == this) {
                    child->transform->parent = nullptr;
                }

                return true;
            }
        }
        return false;
    }

    std::vector<SceneObject *> SceneObject::GetChildren() const {
        if (auto* transform = GetComponent<TransformComponent>()) {
            return transform->children;
        }
        return {};
    }

} // BiBuild