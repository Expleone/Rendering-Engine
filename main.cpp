#include <iostream>
#include <vector>
#include <random>

// Include GLAD before GLFW
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <uuid.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "BiBuild.h"
#include "setup.h"
#include "components/LightComponent.h"
#include "core/RenderSystem.h"
#include "core/ResourceManager.h"
#include "test_models/birds.h"
#include "./test_models/cube.h"
#include "core/TextGenerator.h"
#include "core/Time.h"
#include "gui/ModelLoadGUI.h"
#include "ObjectScripts/BiplaneScript.h"
#include "ObjectScripts/CameraScript.h"
#include "ObjectScripts/FilmScript.h"
#include "ObjectScripts/OutlineScript.h"
#include "ObjectScripts/TextScript.h"
#include "ObjectScripts/TVScript.h"

const int WIN_WIDTH  = 1280;
const int WIN_HEIGHT = 720;
const char* WIN_TITLE  = "GLFW & ImGui - Shaders data";

const int animFrameTimeMs = 150;
bool Debug = false;



void changeMainShader() {
    static bool isChanged = false;
    static auto prevShader = holubiho::ResourceManager::GetShaderProgram("phong");
    if (holubiho::InputManager::IsKeyPressed(GLFW_KEY_P)) {
        if (isChanged) return;
        auto tmp = holubiho::RenderSystem::GetDefaultShader();
        holubiho::RenderSystem::SetDefaultShader(prevShader);
        prevShader = tmp;
        isChanged = true;
    }
    else {
        isChanged = false;
    }
}

void CheckInteraction() {
    static bool intChck = false;
    static bool debugChecked = false;

    if (holubiho::InputManager::IsActionActive("ToggleDebug")) {
        if (!debugChecked){
            Debug = !Debug;
            debugChecked = true;
        }

    } else {
        debugChecked = false;
    }

    if (holubiho::InputManager::GetMouseMode() != GLFW_CURSOR_NORMAL || Debug) return;

    if (holubiho::InputManager::IsActionActive("Interact")) {
        if (intChck) return;
        intChck = true;
        if (auto* obj = holubiho::InputManager::ObjectUnderMouse()) {
            obj->hasBeenInteracted = true;
            std::cout << obj->name<<std::endl;
        }
    }else {
        intChck = false;
    }

}

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
            // Push an ID so ImGui doesn't get confused if two objects have the same name
            ImGui::PushID(i);
            if (scene->objects[i]->GetComponent<holubiho::ModelComponent>()) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255)); // Highlight selected object in yellow
            }
            // Selectable returns true if it was clicked this frame
            if (ImGui::Selectable(scene->objects[i]->name.c_str(), false)) {
                // Handle selection logic here later
                // e.g., selectedObjectIndex = i;
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
    ImGui::DragFloat3("Wind Direction", windDirection, 0.1f, 0.1f, 1000.0f);
    ImGui::End();
    windDir = glm::vec3(windDirection[0], windDirection[1], windDirection[2]);



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



int main() {
    if (!glfwInit()) return -1;


    // Initialize GLAD
    holubiho::SceneManager scene = holubiho::SceneManager();
    holubiho::RenderSystem::Initialize(WIN_WIDTH,WIN_HEIGHT, WIN_TITLE, scene.cameraObject);


    scene.CreateSkyBox(skyboxFaces, nightSkyboxFaces);
    // BiBuild::RenderSystem::SetFogTexture(scene.skybox->GetComponent<BiBuild::ModelComponent>()->mat->textures[0]);
    holubiho::ResourceManager::LoadShaderProgram("phong", "./shaders/vertex/base.vert", "./shaders/fragment/basePhong.frag");
    auto* vegetationShader = holubiho::ResourceManager::LoadShaderProgram("vegetationShader", "./shaders/vertex/vegetation.vert", "./shaders/fragment/base.frag");
    vegetationShader->AddInfo("windDir", &windDir, holubiho::UniformType::Vec3);
    vegetationShader->AddInfo("windStrength", &windStrength, holubiho::UniformType::Float);
    vegetationShader->AddInfo("windSpeed", &windSpeed, holubiho::UniformType::Float);
    vegetationShader->AddInfo("time", holubiho::Time::fCurrentTimePointer(), holubiho::UniformType::Float);

    auto* waterShader = holubiho::ResourceManager::LoadShaderProgram("waterShader", "./shaders/vertex/water.vert", "./shaders/fragment/base.frag");
    waterShader->AddInfo("moveStrength", &waveHeight, holubiho::UniformType::Float);
    waterShader->AddInfo("moveSpeed", &waveSpeed, holubiho::UniformType::Float);
    waterShader->AddInfo("time", holubiho::Time::fCurrentTimePointer(), holubiho::UniformType::Float);


    holubiho::TextGenerator::Init("resources/fonts/arial/ARIAL.TTF");
    SetupInputBindings();


    auto camera = SetupCamera(&scene, &Debug);
    auto cameraScript = camera->GetScript<holubiho::CameraScript>();
    auto cameraComp = camera->GetComponent<holubiho::CameraComponent>();



    auto outline = scene.CreateObject("Outline");
    outline->AddScript<holubiho::OutlineScript>();

    auto tv = SetupTv(&scene);
    cameraScript->SetTVObject(tv);
    // tv->GetScript<BiBuild::TVScript>()->SetLight(SpotlightObj);
    // tv->AddChild(SpotlightObj);
    // SpotlightObj->transform->localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    // SpotlightLight->direction = -tv->transform->Forward();
    // SpotlightLight->cutoff = 90.0f;


    auto sceneObjs = scene.CreateObject("sceneObjs");
    holubiho::ResourceManager::LoadModelsFromFile("resources/scene/Scene.obj", sceneObjs, true);
    auto children = sceneObjs->GetChildren();
    children[4]->GetComponent<holubiho::ModelComponent>()->mat->shader = waterShader;
    children[9]->GetComponent<holubiho::ModelComponent>()->mat->shader = vegetationShader;
    children[7]->GetComponent<holubiho::ModelComponent>()->mat->shader = vegetationShader;
    children[6]->GetComponent<holubiho::ModelComponent>()->mat->shader = vegetationShader;
    sceneObjs->transform->localScale = glm::vec3(10.0f);


    auto plane = SetupPlane(&scene, &Debug);
    cameraScript->SetPlaneObject(plane);

    int framen = 0;
    // Main loop



    auto sign = scene.CreateObject("sign");
    holubiho::ResourceManager::LoadModelsFromFile("resources/Sign.glb", sign, false );
    auto text = scene.CreateObject("text");
    text->hasClickableParts = false;
    std::string textContent = "TV usage:\n- Interact with the green button to \nplay/pause videos.\n- Use the red and orange buttons to \nswitch between videos.\n- Enjoy the show!";
    auto textScript = holubiho::TextGenerator::CreateText(text, textContent, 0.0007, {1.0f, 0, 0});
    sign->AddChild(text);
    text->transform->localPosition = glm::vec3(0.02, 2.429f, 1.135f);
    text->transform->localRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0,1,0));
    sign->transform->localPosition = glm::vec3(-0.283f, -0.047f, -9.593f);
    sign->transform->localRotation = glm::angleAxis(glm::radians(124.0f), glm::vec3(0,1,0));
    sign->transform->localScale = glm::vec3(0.578f);

    auto boat = scene.CreateObject("boat");
    holubiho::ResourceManager::LoadModelsFromFile("resources/low_poly_boat.glb", boat, false);
    boat->transform->localPosition = glm::vec3(-10.123f, 0.069f, -9.469f);
    boat->transform->localRotation = glm::angleAxis(glm::radians(-70.0f), glm::vec3(0,1,0)) * glm::angleAxis(glm::radians(15.0f), glm::vec3(1,0,0));
    boat->transform->localScale = glm::vec3(0.547f);


    while (!glfwWindowShouldClose(holubiho::RenderSystem::GetGLFWWindow())) {
        holubiho::Time::UpdateTime();
        static bool keybiningsReloaded = false;
        if (holubiho::InputManager::IsActionActive("ReloadKeybindings")) {
             if (!keybiningsReloaded) {
                 SetupInputBindings();
                 LoadTVVideosTXT("tv_videos.txt", tv);
                keybiningsReloaded = true;
            }
        }else {
            keybiningsReloaded = false;
        }

        glfwPollEvents();
        CheckInteraction();
        scene.UpdateScene();

        if (++framen/60 >= birds_data.nAnimFrames) {
            framen = 0;
        }

        holubiho::ResourceManager::LoadMesh("bird_mesh", birds_data.vertices + framen/60*birds_data.nVertices*3, birds_data.nVertices*3, birds_data.faces, birds_data.nFaces * 3, glm::vec3(1.0f, 0.5f, 0.2f));
        // changeMainShader();
        holubiho::RenderSystem::UpdateAndDraw(
            scene,
            cameraComp->BuildCameraViewMatrix(),
            cameraComp->GetProjectionMat()
        );
        holubiho::RenderSystem::DrawIDs(scene,
            cameraComp->BuildCameraViewMatrix(),
            cameraComp->GetProjectionMat()
        );
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (Debug) DrawDebug(&scene);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(holubiho::RenderSystem::GetGLFWWindow());
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(holubiho::RenderSystem::GetGLFWWindow());
    glfwTerminate();
    return 0;
}


