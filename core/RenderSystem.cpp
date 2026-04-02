//
// Created by expleoene on 3/30/26.
//

#include "RenderSystem.h"

#include "Mesh.h"
#include "../components/MaterialComponent.h"


namespace BiBuild {

    void RenderSystem::Initialize(GLFWwindow *window, int width, int height, SceneObject* cameraObject, ResourceManager* resourceMngr) {
        screenWidth = width;
        screenHeight = height;
        camera = cameraObject ? cameraObject->GetComponent<CameraComponent>() : nullptr;
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
            if (auto* self = static_cast<RenderSystem*>(glfwGetWindowUserPointer(win))) {
                self->OnWindowResize(win, w, h);
            }
        });
        if (camera) {
            camera->aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        }
        // 1. Configure the Viewport
        // Tells OpenGL the size of the rendering window
        glViewport(0, 0, screenWidth, screenHeight);

        // 2. Configure Global OpenGL State
        // Enable Depth Testing to render objects based on distance, not draw order
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Enable Blending for transparency (useful for grid lines or transparent materials)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        // Tell OpenGL to cull (hide) the back faces
        glCullFace(GL_BACK);

        // Tell OpenGL that Counter-Clockwise vertices are the front faces
        glFrontFace(GL_CCW);

        // Set the default background color (e.g., a dark gray editor background)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        // 3. Compile base assets
        // Call the internal helper to compile the vertex and fragment shaders
        defaultShader = resourceMngr->LoadShaderProgram("default", "../shaders/vertex/base.vert", "../shaders/fragment/base.frag");
        matricesUBO = new UniformBuffer(sizeof(glm::mat4) * 2, static_cast<GLuint>(UBOBinding::Matrices));

        // 4. Initialize persistent editor visuals (Optional)
        // If you have a 3D floor grid, you would generate its VAO/VBO here.
        // SetupFloorGrid();
    }

    RenderSystem::~RenderSystem() {
        // Clean up global resources like shaders and UBOs
        // if (defaultShader) {
        //     delete defaultShader;
        //     defaultShader = nullptr;
        // }
        // if (matricesUBO) {
        //     delete matricesUBO;
        //     matricesUBO = nullptr;
        // }
    }


    void RenderSystem::UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!defaultShader) {
            std::cerr << "Error: Default shader not initialized." << std::endl;
            return;
        }

        // 1. Update global camera matrices ONCE per frame
        struct UniformBlock {
            glm::mat4 view;
            glm::mat4 projection;
        } uboData{ viewMatrix, projectionMatrix };

        matricesUBO->UpdateSubData(&uboData, sizeof(uboData), 0);

        // Track the currently active shader to prevent redundant state changes
        ShaderProgram* activeShader = nullptr;
        drawCallsLastFrame = 0;
        objectsOnScreenLastFrame = static_cast<int>(sceneObjects.size());
        // 2. Render all objects
        for (const std::unique_ptr<SceneObject>& objPtr : sceneObjects) {
            SceneObject* obj = objPtr.get();
            auto* mesh_comp = obj->GetComponent<MeshComponent>();
            auto* material_comp = obj->GetComponent<MaterialComponent>();


            if (!mesh_comp || !mesh_comp->mesh) continue;
            auto* mesh = mesh_comp->mesh;

            // Determine the correct shader for this specific mesh
            ShaderProgram* targetShader = (material_comp!=nullptr && material_comp->shader != nullptr) ? material_comp->shader : defaultShader;

            // Only issue a GL state change if the shader actually changed
            if (activeShader != targetShader) {
                targetShader->Use();
                activeShader = targetShader;
            }

            // Send instance data and draw
            auto modelMatrix = obj->transform->worldMatrix;
            mesh->Draw(modelMatrix, activeShader);
            drawCallsLastFrame++;
        }

    }


    void RenderSystem::OnWindowResize(GLFWwindow* window, int  width, int height) {
        screenWidth = width;
        screenHeight = height;
        glViewport(0, 0, screenWidth, screenHeight);
        if (camera) {
            camera->aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        }
    }
} // BiBuild