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
#include "components/LightComponent.h"
#include "core/RenderSystem.h"
#include "core/ResourceManager.h"
#include "test_models/birds.h"
#include "./test_models/cube.h"
#include "core/Time.h"
#include "gui/ModelLoadGUI.h"
#include "ObjectScripts/CameraScript.h"

const int WIN_WIDTH  = 800;
const int WIN_HEIGHT = 600;
const char* WIN_TITLE  = "GLFW & ImGui - Shaders data";

const int animFrameTimeMs = 150;

static glm::vec3 windDir = glm::vec3(1,0,1);
static float windStrength = 1;
static float windSpeed = 1;


void changeMainShader() {
    static bool isChanged = false;
    static auto prevShader = BiBuild::ResourceManager::GetShaderProgram("phong");
    if (BiBuild::InputManager::IsKeyPressed(GLFW_KEY_P)) {
        if (isChanged) return;
        auto tmp = BiBuild::RenderSystem::GetDefaultShader();
        BiBuild::RenderSystem::SetDefaultShader(prevShader);
        prevShader = tmp;
        isChanged = true;
    }
    else {
        isChanged = false;
    }
}






int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    BiBuild::SceneManager scene = BiBuild::SceneManager();
    BiBuild::RenderSystem::Initialize(window, WIN_WIDTH,WIN_HEIGHT, scene.cameraObject);
    BiBuild::InputManager::Init(window);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");



    auto* camera = scene.cameraObject ? scene.cameraObject->GetComponent<BiBuild::CameraComponent>() : nullptr;
    if (!camera) {
        std::cerr << "CameraComponent is missing on cameraObject." << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    scene.cameraObject->AddScript<BiBuild::CameraScript>();
    scene.cameraObject->transform->localPosition = glm::vec3(0.0f, 00.0f, 10.0f);




    auto sunObj = scene.CreateObject("SunLight");
    auto* sunLight = sunObj->AddComponent<BiBuild::LightComponent>();

    sunLight->type      = BiBuild::LightType::Directional;
    sunLight->direction = glm::vec3(-0.5f, -1.0f, -0.5f); // Angled diagonally down
    sunLight->ambient   = glm::vec3(0.7f, 0.7f, 0.7f);   // Slight blueish tint to global shadows
    sunLight->diffuse   = glm::vec3(0.9f, 0.85f, 0.8f);   // Warm daytime sunlight
    sunLight->specular  = glm::vec3(0.8f, 0.8f, 0.8f);
    sunLight->intensity = 1.0f;




    auto test = scene.CreateObject("test");
    auto tmod = test->AddComponent<BiBuild::ModelComponent>();
    tmod->mesh = BiBuild::ResourceManager::GetMesh("./test_models/Dragon_80K.obj");

    const float dimFactor = 0.2f;

    auto blueMat = BiBuild::ResourceManager::CreateMaterial("WhitishBlue");
    blueMat->diffuse = glm::vec4(0.7f, 0.7f, 1.0f, 1.0f);
    blueMat->ambient = glm::vec3(blueMat->diffuse) * dimFactor;

    auto purpleMat = BiBuild::ResourceManager::CreateMaterial("WhitishPurple");
    purpleMat->diffuse = glm::vec4(0.8f, 0.6f, 1.0f, 1.0f);
    purpleMat->ambient = glm::vec3(purpleMat->diffuse) * dimFactor;

    auto greenMat = BiBuild::ResourceManager::CreateMaterial("WhitishGreen");
    greenMat->diffuse = glm::vec4(0.7f, 1.0f, 0.7f, 1.0f);
    greenMat->ambient = glm::vec3(greenMat->diffuse) * dimFactor;

    auto pinkMat = BiBuild::ResourceManager::CreateMaterial("WhitishPink");
    pinkMat->diffuse = glm::vec4(1.0f, 0.6f, 0.8f, 1.0f);
    pinkMat->ambient = glm::vec3(pinkMat->diffuse) * dimFactor;

    auto yellowMat = BiBuild::ResourceManager::CreateMaterial("WhitishYellow");
    yellowMat->diffuse = glm::vec4(1.0f, 1.0f, 0.6f, 1.0f);
    yellowMat->ambient = glm::vec3(yellowMat->diffuse) * dimFactor;

    auto redMat = BiBuild::ResourceManager::CreateMaterial("WhitishRed");
    redMat->diffuse = glm::vec4(1.0f, 0.6f, 0.6f, 1.0f);
    redMat->ambient = glm::vec3(redMat->diffuse) * dimFactor;

    auto cyanMat = BiBuild::ResourceManager::CreateMaterial("WhitishCyan");
    cyanMat->diffuse = glm::vec4(0.6f, 1.0f, 1.0f, 1.0f);
    cyanMat->ambient = glm::vec3(cyanMat->diffuse) * dimFactor;

    int framen = 0;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        BiBuild::Time::UpdateTime();
        glfwPollEvents();

        scene.UpdateScene();

        if (++framen/60 >= birds_data.nAnimFrames) {
            framen = 0;
        }

        changeMainShader();
        BiBuild::RenderSystem::UpdateAndDraw(
            scene,
            camera->BuildCameraViewMatrix(),
            camera->GetProjectionMat()
        );

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");
        ImGui::SliderFloat("Zoom", &camera->fov, 1.0f, 180.0f);
        ImGui::SliderFloat("Speed", &camera->movementSpeed, 1.0f, 500.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        BiBuild::ModelLoadGUI::draw(test);

        ImGui::Begin("Scene Objects");

        for (int i = 0; i < scene.objects.size(); ++i) {
            if (scene.objects[i]) {
                ImGui::PushID(i);
                if (scene.objects[i]->GetComponent<BiBuild::ModelComponent>()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                }
                if (ImGui::Selectable(scene.objects[i]->name.c_str(), false)) {
                    // Handle selection logic here later
                    // e.g., selectedObjectIndex = i;
                }
                if (scene.objects[i]->GetComponent<BiBuild::ModelComponent>()) {
                    ImGui::PopStyleColor();
                }
                ImGui::PopID();
            }
        }

        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}