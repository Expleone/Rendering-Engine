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

    const float dimFactor = 0.2f;




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

    static std::vector<BiBuild::Material*> GetWhitishMaterials() {
        std::vector<BiBuild::Material*> materials(7);

        materials[0] = BiBuild::ResourceManager::GetMaterial("WhitishBlue");
        materials[1] = BiBuild::ResourceManager::GetMaterial("WhitishPurple");
        materials[2] = BiBuild::ResourceManager::GetMaterial("WhitishGreen");
        materials[3] = BiBuild::ResourceManager::GetMaterial("WhitishPink");
        materials[4] = BiBuild::ResourceManager::GetMaterial("WhitishYellow");
        materials[5] = BiBuild::ResourceManager::GetMaterial("WhitishRed");
        materials[6] = BiBuild::ResourceManager::GetMaterial("WhitishCyan");

        return materials;
    }

    static void loadModel(SceneObject* obj, const std::string& newFilepath) {

        auto newMeshes = ResourceManager::LoadMeshesFromFile(newFilepath);
        if (newMeshes.empty()) return;
        auto mats = GetWhitishMaterials();
        obj->DeleteAllComponents<ModelComponent>();
        int i = 7;
        for (auto mesh : newMeshes) {
            auto model = obj->AddComponent<ModelComponent>();
            model->mesh = mesh;
            if (i != 7) model->mat = mats[i];
            i--;
            if (i <= 0) i = 7;
        }
        scale = 1;
        filepath = newFilepath;
    }
};

} // BiBuild

#endif //VIEWER_MODELLOADGUI_H
