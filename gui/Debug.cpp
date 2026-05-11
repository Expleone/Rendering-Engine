#include "Debug.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../core/SceneObject.h"
#include "../core/SceneManager.h"
#include "../core/RenderSystem.h"
#include "../core/InputHandler.h"
#include "../setup.h"



//
// Created by expleoene on 5/11/26.
//


void DrawDebug(holubiho::SceneManager* scene) {
    static bool clickEvaluated = false;
    static holubiho::SceneObject* currentObject = nullptr;
    static float pos[3] = {0,0,0};
    static float rotation[3] = {0,0,0};
    static float scale = 0.01f;
    static float posStep = 0.001f;
    static float scaleStep = 0.00001f;
    static float rotationStep = 1.0f;
    static float windDirection[3] = {windDir.x, windDir.y, windDir.z};
    static bool DrawUUID = false;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Scene Objects");

    for (int i = 0; i < scene->objects.size(); ++i) {
        if (scene->objects[i]) {
            ImGui::PushID(i);
            if (scene->objects[i]->GetComponent<holubiho::ModelComponent>()) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
            }
            if (ImGui::Selectable(scene->objects[i]->name.c_str(), false)) {
                currentObject = scene->objects[i].get();
                pos[0] = currentObject->transform->localPosition.x;
                pos[1] = currentObject->transform->localPosition.y;
                pos[2] = currentObject->transform->localPosition.z;
                scale = currentObject->transform->localScale.x;

                glm::vec3 euler = glm::degrees(glm::eulerAngles(currentObject->transform->localRotation));
                rotation[0] = euler.x;
                rotation[1] = euler.y;
                rotation[2] = euler.z;
            }
            if (scene->objects[i]->GetComponent<holubiho::ModelComponent>()) {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();
        }
    }

    ImGui::End();

    if (holubiho::InputManager::IsActionActive("Interact") && holubiho::InputManager::GetMouseMode() == GLFW_CURSOR_NORMAL && !io.WantCaptureMouse) {
        if (!clickEvaluated) {
            clickEvaluated = true;

            currentObject = holubiho::InputManager::ObjectUnderMouse();
            if (!currentObject) return;
            pos[0] = currentObject->transform->localPosition.x;
            pos[1] = currentObject->transform->localPosition.y;
            pos[2] = currentObject->transform->localPosition.z;
            scale = currentObject->transform->localScale.x;

            glm::vec3 euler = glm::degrees(glm::eulerAngles(currentObject->transform->localRotation));
            rotation[0] = euler.x;
            rotation[1] = euler.y;
            rotation[2] = euler.z;
        }
    } else {
        clickEvaluated = false;
    }


    if (currentObject) {
        ImGui::Begin("Tranform Debug ");
        ImGui::Text("Selected Object: %s", currentObject->name.c_str());
        ImGui::DragFloat3("Pos", pos,  posStep, -10000.0f, 10000.0f);
        ImGui::DragFloat3("Rot (Deg)", rotation, rotationStep, -360.0f, 360.0f);
        ImGui::DragFloat("Scale", &scale, scaleStep, 0.00001f, 1000.0f);
        ImGui::DragFloat("Pos Step", &posStep, 0.0001f, 0.00001f, 1.0f);
        ImGui::DragFloat("Rot Step", &rotationStep, 0.001f, 0.001f, 180.0f);
        ImGui::DragFloat("Scale Step", &scaleStep, 0.00001f, 0.000001f, 0.1f);
        ImGui::End();

        currentObject->transform->localPosition = glm::vec3(pos[0], pos[1], pos[2]);
        currentObject->transform->localScale = glm::vec3(scale);

        glm::vec3 radEuler = glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2]));
        currentObject->transform->localRotation = glm::quat(radEuler);
    }

    ImGui::Begin("Scene parameters ");
    ImGui::DragFloat("Waves Height", &waveHeight, 0.01, 0.01f, 10.0f);
    ImGui::DragFloat("Wave Speed", &waveSpeed, 0.01f, 0.001f, 10.0f);
    ImGui::DragFloat("Wind Strength", &windStrength, 0.01, 0.01f, 10.0f);
    ImGui::DragFloat("Wind Speed", &windSpeed, 0.01f, 0.001f, 10.0f);
    if (ImGui::DragFloat3("Wind Direction", windDirection, 0.1f, -1000.0f, 1000.0f)) {
        windDir = glm::vec3(windDirection[0], windDirection[1], windDirection[2]);
    }
    ImGui::End();
    windDirection[0] = windDir.x;
    windDirection[1] = windDir.y;
    windDirection[2] = windDir.z;



    holubiho::CameraComponent* camera = scene->cameraObject ? scene->cameraObject->GetComponent<holubiho::CameraComponent>() : nullptr;

    if (!camera) return;
    auto cameraScript =  camera->GetOwner()->GetScript<holubiho::CameraScript>();
    if (!cameraScript) return;
    ImGui::Begin("Controls");
    ImGui::SliderFloat("Zoom", &camera->fov, 1.0f, 180.0f);
    ImGui::SliderFloat("Speed", cameraScript->GetCameraSpeedPtr(), 1.0f, 500.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    static bool actionChecked = false;
    if (holubiho::InputManager::IsActionActive("ToggleUUID")) {
        if (!actionChecked) {
            DrawUUID = !DrawUUID;
            actionChecked = true;
        }
    } else {
        actionChecked = false;
    }
    if (DrawUUID) {
        holubiho::RenderSystem::DrawFullscreenQuad(holubiho::RenderSystem::GetUUIDFrameBuffer()->GetTexture());
    }

}