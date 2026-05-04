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

        if (!obj) {
            ImGui::TextUnformatted("No scene object selected.");
            ImGui::End();
            return;
        }

        auto model = obj->GetComponent<ModelComponent>();
        if (!model) model = obj->AddComponent<ModelComponent>();
        if (model && model->mesh) {
            ImGui::Text("Current model: %s", model->mesh->name.c_str());
        } else {
            ImGui::TextUnformatted("Current model: <none>");
        }
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
        if (!obj) return;
        auto model = obj->GetComponent<ModelComponent>();
        if (!model) model = obj->AddComponent<ModelComponent>();
        if (!model) return;

        auto newMesh = ResourceManager::GetMesh(filepath);
        if (!newMesh) return;
        scale = 1;
        model->mesh = newMesh;
    }
};

} // BiBuild

#endif //VIEWER_MODELLOADGUI_H
