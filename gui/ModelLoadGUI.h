//
// Created by expleoene on 4/16/26.
//

#ifndef VIEWER_MODELLOADGUI_H
#define VIEWER_MODELLOADGUI_H
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "../components/ModelComponent.h"
#include "../core/ResourceManager.h"
#include "../core/SceneObject.h"

namespace BiBuild {

class ModelLoadGUI {

    static std::string filepath;
    static float scale;
    static float oldScale;
public:
    static void draw(SceneObject* obj) {
        ImGui::Begin("Load Model");

        if (!obj) return;
        auto model = obj->GetComponent<ModelComponent>();
        if (!model) model = obj->AddComponent<ModelComponent>();
        ImGui::Text("Current model: %s", model->mesh->name.c_str());
        ImGui::InputText("Filepath:", &filepath);
        if (ImGui::Button("Load")) {
            loadModel(obj, filepath);
        }

        ImGui::DragFloat("Scale", &scale,0.5, 0.01, 1000.0f);
        if (oldScale != scale) {
            obj->transform->localScale = glm::vec3(scale);
            oldScale = scale;
        }
        ImGui::End();
    }

    static void loadModel(SceneObject* obj, std::string& filepath) {
        auto model = obj->GetComponent<ModelComponent>();
        auto newMesh = ResourceManager::GetMesh(filepath);
        if (!newMesh) return;
        scale = 1;
        model->mesh = newMesh;
    }
};

} // BiBuild

#endif //VIEWER_MODELLOADGUI_H
