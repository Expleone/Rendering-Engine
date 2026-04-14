//
// Created by expleoene on 3/30/26.
//

#include "RenderSystem.h"

#include "Mesh.h"
#include "../components/LightComponent.h"


namespace BiBuild {

    void RenderSystem::Initialize(GLFWwindow *window, int width, int height, SceneObject* cameraObject) {
        Get().screenWidth = width;
        Get().screenHeight = height;
        Get().camera = cameraObject ? cameraObject->GetComponent<CameraComponent>() : nullptr;
        glfwSetWindowUserPointer(window, &Get());
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
            if (auto* self = static_cast<RenderSystem*>(glfwGetWindowUserPointer(win))) {
                self->OnWindowResize(win, w, h);
            }
        });
        if (Get().camera) {
            Get().camera->aspectRatio = static_cast<float>(Get().screenWidth) / static_cast<float>(Get().screenHeight);
        }
        glViewport(0, 0, Get().screenWidth, Get().screenHeight);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);

        glFrontFace(GL_CCW);

        // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Get().defaultShader = ResourceManager::LoadShaderProgram("default", "../shaders/vertex/base.vert", "../shaders/fragment/base.frag");
        Get().matricesUBO = new UniformBuffer(sizeof(glm::mat4) * 2, static_cast<GLuint>(UBOBinding::Matrices));


        Get().lightsUBO = new UniformBuffer(sizeof(LightsUBOStructure), static_cast<GLuint>(UBOBinding::Lights));
    }

    RenderSystem::~RenderSystem() = default;


    void RenderSystem::UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!Get().defaultShader) {
            std::cerr << "Error: Default shader not initialized." << std::endl;
            return;
        }

        struct UniformBlock {
            glm::mat4 view;
            glm::mat4 projection;
        } uboData{ viewMatrix, projectionMatrix };

        Get().matricesUBO->UpdateSubData(&uboData, sizeof(uboData), 0);

        int lightCount = 0;
        for (const std::unique_ptr<SceneObject>& objPtr : sceneObjects) {
            if (lightCount >= 100) break; // Prevent overflow of our UBO array
            SceneObject* obj = objPtr.get();
            auto* light_comp = obj->GetComponent<LightComponent>();
            if (!light_comp) continue;
            glm::vec3 position = glm::vec3(obj->transform->worldMatrix[3]);
            Get().lightsUBOStruct.lights[lightCount].position = glm::vec4(position,static_cast<float>(light_comp->type));
            Get().lightsUBOStruct.lights[lightCount].ambient = glm::vec4(light_comp->ambient, light_comp->direction.x);
            Get().lightsUBOStruct.lights[lightCount].diffuse = glm::vec4(light_comp->diffuse, light_comp->direction.y);
            Get().lightsUBOStruct.lights[lightCount].specular = glm::vec4(light_comp->specular, light_comp->direction.z);
            Get().lightsUBOStruct.lights[lightCount].attenuation = glm::vec4(light_comp->attenuation, glm::radians(light_comp->cutoff));
            lightCount++;
        }
        Get().lightsUBOStruct.numLights = lightCount;
        Get().lightsUBO->UpdateSubData(&Get().lightsUBOStruct, sizeof(Get().lightsUBOStruct), 0);

        // Track the currently active shader to prevent redundant state changes
        ShaderProgram* activeShader = nullptr;
        Get().drawCallsLastFrame = 0;
        Get().objectsOnScreenLastFrame = static_cast<int>(sceneObjects.size());
        // 2. Render all objects
        for (const std::unique_ptr<SceneObject>& objPtr : sceneObjects) {
            SceneObject* obj = objPtr.get();
            if (!obj) continue;

            auto* model_comp = obj->GetComponent<ModelComponent>();
            if (!model_comp || !model_comp->mesh) continue;

            auto* material = model_comp->mat;

            ShaderProgram* targetShader = (material!=nullptr && material->shader != nullptr) ? material->shader : Get().defaultShader;

            if (activeShader != targetShader) {
                targetShader->Use();
                activeShader = targetShader;
            }

            model_comp->Draw(targetShader);

            Get().drawCallsLastFrame++;
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

    ShaderProgram *RenderSystem::GetDefaultShader() {
        return Get().defaultShader;
    }
} // BiBuild