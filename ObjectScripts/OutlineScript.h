//
// Created by expleoene on 4/29/26.
//

#ifndef VIEWER_OUTLINESCRIPT_H
#define VIEWER_OUTLINESCRIPT_H
#include "ObjectScript.h"
#include "../components/ModelComponent.h"
#include "../core/ResourceManager.h"

namespace holubiho {
    class OutlineScript : public ObjectScript{
        ModelComponent* model;
    public:
        OutlineScript(SceneObject* owner) : ObjectScript(owner) {
            model = owner->GetComponent<ModelComponent>();
            if (!model) {
                model = owner->AddComponent<ModelComponent>();
            }
            model->cullFront = true;
            model->drawUUID = false;
            model->mat = ResourceManager::CreateMaterial("outlineMat");
            model->mat->shader = ResourceManager::LoadShaderProgram("outlineShader", "./shaders/vertex/outline.vert", "./shaders/fragment/outline.frag");
            owner->transform->updateWorldMatrix = false;
        }

        void Update() override;
    };
} // BiBuild

#endif //VIEWER_OUTLINESCRIPT_H
