//
// Created by expleoene on 3/30/26.
//

#include "RenderSystem.h"

#include "Mesh.h"
#include "../components/MaterialComponent.h"
#include "../components/LightComponent.h"


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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);

        glFrontFace(GL_CCW);

        // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // 3. Compile base assets
        // Call the internal helper to compile the vertex and fragment shaders
        defaultShader = resourceMngr->LoadShaderProgram("default", "../shaders/vertex/base.vert", "../shaders/fragment/base.frag");
        matricesUBO = new UniformBuffer(sizeof(glm::mat4) * 2, static_cast<GLuint>(UBOBinding::Matrices));


        lightsUBO = new UniformBuffer(sizeof(LightsUBOStructure), static_cast<GLuint>(UBOBinding::Lights));

        // 4. Initialize persistent editor visuals (Optional)
        // If you have a 3D floor grid, you would generate its VAO/VBO here.
        // SetupFloorGrid();
    }

    RenderSystem::~RenderSystem() = default;


    void RenderSystem::UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!defaultShader) {
            std::cerr << "Error: Default shader not initialized." << std::endl;
            return;
        }

        struct UniformBlock {
            glm::mat4 view;
            glm::mat4 projection;
        } uboData{ viewMatrix, projectionMatrix };

        matricesUBO->UpdateSubData(&uboData, sizeof(uboData), 0);

        int lightCount = 0;
        for (const std::unique_ptr<SceneObject>& objPtr : sceneObjects) {
            if (lightCount >= 100) break; // Prevent overflow of our UBO array
            SceneObject* obj = objPtr.get();
            auto* light_comp = obj->GetComponent<LightComponent>();
            if (!light_comp) continue;
            glm::vec3 position = glm::vec3(obj->transform->worldMatrix[3]);
            lightData.lights[lightCount].position = glm::vec4(position,static_cast<float>(light_comp->type));
            lightData.lights[lightCount].ambient = glm::vec4(light_comp->ambient, light_comp->direction.x);
            lightData.lights[lightCount].diffuse = glm::vec4(light_comp->diffuse, light_comp->direction.y);
            lightData.lights[lightCount].specular = glm::vec4(light_comp->specular, light_comp->direction.z);
            lightData.lights[lightCount].attenuation = glm::vec4(light_comp->attenuation, glm::radians(light_comp->cutoff));
            lightCount++;
        }
        lightData.numLights = lightCount;
        lightsUBO->UpdateSubData(&lightData, sizeof(lightData), 0);

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
            // std::cout << "Drawing object: " << obj->name << " with shader: " << (activeShader == defaultShader ? "Default" : "Custom") << std::endl;
            if (material_comp) material_comp->SendToShader(activeShader);
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