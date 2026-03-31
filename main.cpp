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
#include "core/RenderSystem.h"
#include "test_models/birds.h"

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
    BiBuild::RenderSystem render_system;

    render_system.Initialize(window, WIN_WIDTH,WIN_HEIGHT,scene.cameraObject);
    // Callbacks setup
    BiBuild::InputManager::Init(window);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    auto obj = scene.CreateObject("bird");
    auto* mesh = obj->GetComponent<BiBuild::MeshComponent>();
    if (!mesh) {
        mesh = obj->AddComponent<BiBuild::MeshComponent>();
    }

    for (int i = 0; i < birds_data.nVertices; ++i) {
        const int base = i * 3;
        mesh->vertices.emplace_back(BiBuild::Vertex{
            glm::vec3(birds_data.vertices[base], birds_data.vertices[base + 1], birds_data.vertices[base + 2]),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        });
    }

    const int indexCount = birds_data.nFaces * 3;
    mesh->indices.reserve(static_cast<size_t>(indexCount));
    for (int i = 0; i < indexCount; ++i) {
        mesh->indices.push_back(static_cast<unsigned int>(birds_data.faces[i]));
    }

    obj->transform->localPosition = glm::vec3(0.0f, 0.0f, -4.0f);

    auto obj2 = scene.CreateObject("bird2");
    auto* mesh2 = obj2->GetComponent<BiBuild::MeshComponent>();
    if (!mesh2) {
        mesh2 = obj2->AddComponent<BiBuild::MeshComponent>();
    }

    for (int i = 0; i < birds_data.nVertices; ++i) {
        const int base = i * 3;
        mesh2->vertices.emplace_back(BiBuild::Vertex{
            glm::vec3(birds_data.vertices[base], birds_data.vertices[base + 1], birds_data.vertices[base + 2]),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        });
    }

    mesh2->indices.reserve(static_cast<size_t>(indexCount));
    for (int i = 0; i < indexCount; ++i) {
        mesh2->indices.push_back(static_cast<unsigned int>(birds_data.faces[i]));
    }

    obj2->transform->localPosition = glm::vec3(1000.0f, 0.0f, -40.0f);

    scene.UpdateScene();

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
    scene.cameraObject->transform->localPosition = glm::vec3(0.0f, 0.0f, 4.0f);

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

        for (int i = 0; i < birds_data.nVertices; ++i) {
            const int base = i * 3 + (framen/60 * birds_data.nVertices * 3);
            mesh2->vertices[i].position = glm::vec3(
                birds_data.vertices[base],
                birds_data.vertices[base + 1],
                birds_data.vertices[base + 2]
            );
        }
        mesh2->isDirty = true;

        render_system.UpdateAndDraw(
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
        // ImGui::SliderFloat("bird", &obj2->transform->localPosition.x, 1, 110000.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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