//
// Created by expleoene on 4/29/26.
//

#include "OutlineScript.h"

#include "../core/InputHandler.h"

namespace BiBuild {
    void OutlineScript::Update() {
        if (InputManager::GetMouseMode() == GLFW_CURSOR_DISABLED) {model->mesh = nullptr; return;}
        auto objToOutline = InputManager::ObjectUnderMouse();
        if (!objToOutline) {model->mesh = nullptr; return;}
        owner->transform->worldMatrix = objToOutline->transform->worldMatrix;
        auto modelToOutline = objToOutline->GetComponent<ModelComponent>();
        if (!modelToOutline) return;
        model->mesh = modelToOutline->mesh;

    }
} // BiBuild