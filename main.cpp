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
#include "components/MaterialComponent.h"
#include "core/RenderSystem.h"
#include "core/ResourceManager.h"
#include "test_models/birds.h"
#include "./test_models/cube.h"

const int WIN_WIDTH  = 800;
const int WIN_HEIGHT = 600;
const char* WIN_TITLE  = "GLFW & ImGui - Shaders data";

const int animFrameTimeMs = 150;




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
    BiBuild::ResourceManager resourceManager;
    BiBuild::RenderSystem renderSystem;
    resourceManager.SetScene(&scene);


    renderSystem.Initialize(window, WIN_WIDTH,WIN_HEIGHT,scene.cameraObject, &resourceManager);
    // Callbacks setup
    BiBuild::InputManager::Init(window);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    auto* bird_mesh = resourceManager.LoadMesh("bird_mesh", birds_data.vertices, birds_data.nVertices*3, birds_data.faces, birds_data.nFaces * 3, glm::vec3(1.0f, 0.5f, 0.2f));

    // auto obj = scene.CreateObject("bird");
    // auto* mesh = obj->GetComponent<BiBuild::MeshComponent>();
    // if (!mesh) {
    //     mesh = obj->AddComponent<BiBuild::MeshComponent>();
    // }
    // mesh->mesh = bird_mesh;
    // auto* material = obj->GetComponent<BiBuild::MaterialComponent>();
    // if (!material) {
    //     material = obj->AddComponent<BiBuild::MaterialComponent>();
    // }
    // // 2. Set up a "Shiny Copper" Material
    // material->ambient   = glm::vec3(0.2f, 0.1f, 0.05f); // Deep, warm shadows
    // material->diffuse   = glm::vec3(1.0f, 0.5f, 0.2f);  // Base orange/copper color
    // material->specular  = glm::vec3(1.0f, 0.8f, 0.6f);  // Bright, warm specular highlight
    // material->emission  = glm::vec3(0.0f);              // Set to e.g., (0.5, 0.1, 0.0) if you want it to glow!
    // material->shininess = 64.0f;

    auto obj2 = scene.CreateObject("bird2");
    auto* mesh2 = obj2->GetComponent<BiBuild::MeshComponent>();
    if (!mesh2) {
        mesh2 = obj2->AddComponent<BiBuild::MeshComponent>();
    }
    mesh2->mesh = bird_mesh;
    auto* material2 = obj2->GetComponent<BiBuild::MaterialComponent>();
    if (!material2) {
        material2 = obj2->AddComponent<BiBuild::MaterialComponent>();
    }
    material2->shader = resourceManager.LoadShaderProgram("bird_shader", "../shaders/vertex/base.vert", "../shaders/fragment/test.frag");
    obj2->transform->localPosition = glm::vec3(1000.0f, 0.0f, 00.0f);

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
    scene.cameraObject->transform->localPosition = glm::vec3(0.0f, 00.0f, 10.0f);






    // 2. Load the mesh
    // auto cube = scene.CreateObject("Cube");
    // auto* cubeMesh = cube->AddComponent<BiBuild::MeshComponent>();
    //
    // cubeMesh->mesh = resourceManager.LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));
    // auto* cubeMaterial = cube->GetComponent<BiBuild::MaterialComponent>();
    // if (!cubeMaterial) {
    //     cubeMaterial = cube->AddComponent<BiBuild::MaterialComponent>();
    // }
    // // 2. Set up a "Shiny Copper" Material
    // cubeMaterial->ambient   = glm::vec3(0.2f, 0.1f, 0.05f); // Deep, warm shadows
    // cubeMaterial->diffuse   = glm::vec3(1.0f, 0.5f, 0.2f);  // Base orange/copper color
    // cubeMaterial->specular  = glm::vec3(1.0f, 0.8f, 0.6f);  // Bright, warm specular highlight
    // cubeMaterial->emission  = glm::vec3(0.0f);              // Set to e.g., (0.5, 0.1, 0.0) if you want it to glow!
    // cubeMaterial->shininess = 64.0f;


    auto sunObj = scene.CreateObject("SunLight");
    auto* sunLight = sunObj->AddComponent<BiBuild::LightComponent>();

    sunLight->type      = BiBuild::LightType::Directional;
    sunLight->direction = glm::vec3(-0.5f, -1.0f, -0.5f); // Angled diagonally down
    sunLight->ambient   = glm::vec3(0.1f, 0.1f, 0.15f);   // Slight blueish tint to global shadows
    sunLight->diffuse   = glm::vec3(0.9f, 0.85f, 0.8f);   // Warm daytime sunlight
    sunLight->specular  = glm::vec3(1.0f, 1.0f, 1.0f);
    sunLight->intensity = 1.0f;


    auto pointObj = scene.CreateObject("BlueRimLight");
    auto* pointMesh = pointObj->AddComponent<BiBuild::MeshComponent>();
    pointMesh->mesh = resourceManager.LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));
    auto* pointMaterial = pointObj->GetComponent<BiBuild::MaterialComponent>();
    if (!pointMaterial) {
        pointMaterial = pointObj->AddComponent<BiBuild::MaterialComponent>();
    }
    pointMaterial->ambient   = glm::vec3(0.0f); // No self-illumination
    pointMaterial->diffuse   = glm::vec3(0.0f); //
    pointMaterial->specular  = glm::vec3(0.0f); // No specular highlights on the cube itself
    pointMaterial->emission  = glm::vec3(0.1f, 0.6f, 1.0f); // Bright cyan glow to visualize the light
    auto* pointLight = pointObj->AddComponent<BiBuild::LightComponent>();
    pointLight->type        = BiBuild::LightType::Point;
    pointLight->ambient     = glm::vec3(0.0f);
    pointLight->diffuse     = glm::vec3(0.1f, 0.6f, 1.0f);  // Bright teal/cyan to contrast the orange bird
    pointLight->specular    = glm::vec3(0.5f, 0.8f, 1.0f);
    pointLight->attenuation = glm::vec3(1.0f, 0.0014f, 0.000007f); // Fades out nicely
    // pointLight->intensity   = 2.0f; // Overdrive the intensity slightly for a dramatic rim effect
    pointObj->transform->localPosition = glm::vec3(0.0f, 1000.0f, 00.0f); // Positioned above and behind the camera for rim lighting
    pointObj->transform->localScale = glm::vec3(0.2f); // Make the cube smaller to represent the light source

    auto dragon = scene.CreateObject("dragon");
    auto* dragonMesh = dragon->AddComponent<BiBuild::MeshComponent>();

    dragonMesh->mesh = resourceManager.GetMesh("../test_models/Dragon_80K.obj");
    auto* dragonMaterial = dragon->GetComponent<BiBuild::MaterialComponent>();
    if (!dragonMaterial) {
        dragonMaterial = dragon->AddComponent<BiBuild::MaterialComponent>();
    }
    // 2. Set up a "Shiny Copper" Material
    dragonMaterial->ambient   = glm::vec3(0.2f, 0.1f, 0.05f); // Deep, warm shadows
    dragonMaterial->diffuse   = glm::vec3(1.0f, 0.5f, 0.2f);  // Base orange/copper color
    dragonMaterial->specular  = glm::vec3(1.0f, 0.8f, 0.6f);  // Bright, warm specular highlight
    dragonMaterial->emission  = glm::vec3(0.0f);              // Set to e.g., (0.5, 0.1, 0.0) if you want it to glow!
    dragonMaterial->shininess = 10.0f;
    dragon->transform->localScale = glm::vec3(100);
    // pointMesh->mesh = resourceManager.GetMesh("../test_models/Dragon_80K.obj");
    // CameraControlState cameraLookState;
    float lastFrameTime = static_cast<float>(glfwGetTime());
    int framen = 0;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (!ImGui::GetIO().WantCaptureKeyboard) {
            camera->UpdateCameraFromInput(deltaTime);
        }
        scene.UpdateScene();

        if (++framen/60 >= birds_data.nAnimFrames) {
            framen = 0;
        }

        resourceManager.LoadMesh("bird_mesh", birds_data.vertices + framen/60*birds_data.nVertices*3, birds_data.nVertices*3, birds_data.faces, birds_data.nFaces * 3, glm::vec3(1.0f, 0.5f, 0.2f));

        renderSystem.UpdateAndDraw(
            scene.objects,
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
        ImGui::SliderFloat("lightX", &pointObj->transform->localPosition.x, -20, 20.0f);
        ImGui::SliderFloat("lightY", &pointObj->transform->localPosition.y, -20, 20.0f);
        ImGui::SliderFloat("lightZ", &pointObj->transform->localPosition.z, -20, 20.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Draw calls per frame: %d; Objects on the scene: %d", renderSystem.drawCallsLastFrame, renderSystem.objectsOnScreenLastFrame);
        ImGui::End();

        ImGui::Begin("Scene Objects");

        for (int i = 0; i < scene.objects.size(); ++i) {
            if (scene.objects[i]) {
                // Push an ID so ImGui doesn't get confused if two objects have the same name
                ImGui::PushID(i);
                if (scene.objects[i]->GetComponent<BiBuild::MeshComponent>()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255)); // Highlight selected object in yellow
                }
                // Selectable returns true if it was clicked this frame
                if (ImGui::Selectable(scene.objects[i]->name.c_str(), false)) {
                    // Handle selection logic here later
                    // e.g., selectedObjectIndex = i;
                }
                if (scene.objects[i]->GetComponent<BiBuild::MeshComponent>()) {
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