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

        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);

        glfwWindowHint(GLFW_DEPTH_BITS, 24);

        glfwWindowHint(GLFW_STENCIL_BITS, 8);

        glViewport(0, 0, Get().screenWidth, Get().screenHeight);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);

        glFrontFace(GL_CCW);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


        Get().defaultShader = ResourceManager::LoadShaderProgram("default", "./shaders/vertex/base.vert", "./shaders/fragment/base.frag");
        Get().matricesUBO = new UniformBuffer(sizeof(glm::mat4) * 2, static_cast<GLuint>(UBOBinding::Matrices));
        Get().lightsUBO = new UniformBuffer(sizeof(LightsUBOStructure), static_cast<GLuint>(UBOBinding::Lights));
        Get().fogUBO = new UniformBuffer(sizeof(FogUBOStructure), static_cast<GLuint>(UBOBinding::Fog));
    }

    RenderSystem::~RenderSystem() = default;


    void RenderSystem::UpdateAndDraw(const SceneManager& scene, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        // std::vector<std::unique_ptr<SceneObject>> sceneObjects = scene.objects;
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
        for (const std::unique_ptr<SceneObject>& objPtr : scene.objects) {
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
        Get().fogUBO->UpdateData(&Get().fogUBOStruct, sizeof(Get().fogUBOStruct));

        // Track the currently active shader to prevent redundant state changes
        ShaderProgram* activeShader = nullptr;
        Get().drawCallsLastFrame = 0;
        Get().objectsOnScreenLastFrame = static_cast<int>(scene.objects.size());

        // 1. Render skybox first (disable depth write)
        if (scene.skybox) {
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);

            auto* model_comp = scene.skybox->GetComponent<ModelComponent>();
            if (model_comp && model_comp->mesh){
                auto* material = model_comp->mat;
                ShaderProgram* targetShader = (material!=nullptr && material->shader != nullptr) ? material->shader : Get().defaultShader;

                if (activeShader != targetShader) {
                    targetShader->Use();
                    activeShader = targetShader;
                }
                targetShader->SendAdditionalInfo();
                model_comp->Draw(targetShader);
                Get().drawCallsLastFrame++;


            }
            // 2. Re-enable depth writes and restore depth function
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);

        }

        // 3. Render all other objects
        for (const std::unique_ptr<SceneObject>& objPtr : scene.objects) {
            SceneObject* obj = objPtr.get();
            if (!obj || obj->name == "Skybox") continue;

            std::vector<ModelComponent*> models = obj->GetAllComponents<ModelComponent>();

            for (auto& model_comp : models) {
                if (!model_comp || !model_comp->mesh) continue;

                auto* material = model_comp->mat;

                ShaderProgram* targetShader = (material!=nullptr && material->shader != nullptr) ? material->shader : Get().defaultShader;

                if (activeShader != targetShader) {
                    targetShader->Use();
                    activeShader = targetShader;
                }
                //Send additional uniforms to shaders
                targetShader->SendAdditionalInfo();
                // targetShader->SetUniformFloat()
                model_comp->Draw(targetShader);

                Get().drawCallsLastFrame++;
            }
            // auto* model_comp = obj->GetComponent<ModelComponent>();

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

    void RenderSystem::SetDefaultShader(ShaderProgram* prg) {
        Get().defaultShader = prg;
    }

    void RenderSystem::SetFogTexture(Texture *tex) {
        Get().fogTex = tex;
        Get().fogUBOStruct.useTex = true;
    }

    Texture *RenderSystem::GetFogTexture() {
        if (Get().fogUBOStruct.useTex) {
            return  Get().fogTex;
        }
        return nullptr;
    }

    void RenderSystem::SetUseSkyboxTexAsFog(bool use) {
        Get().useSkyboxTexAsFog = use;
    }

    bool RenderSystem::GetUseSkyboxTexAsFog() {
        return Get().useSkyboxTexAsFog;
    }

    void RenderSystem::SetFogSettings(FogUBOStructure settings) {
        Get().fogUBOStruct = settings;
    }

    void RenderSystem::SetFogColor(glm::vec3 color) {
        Get().fogUBOStruct.color = glm::vec4(color,1);
    }

    void RenderSystem::SetFogDistance(float distanceClose, float distanceFar) {
        Get().fogUBOStruct.distClose = distanceClose;
        Get().fogUBOStruct.distFar = distanceFar;
    }

    void RenderSystem::SetFogSettings(glm::vec3 color, float distanceClose, float distanceFar, bool useTex) {
        Get().fogUBOStruct = {glm::vec4(color,1), distanceClose, distanceFar, useTex};
    }

    void RenderSystem::SetFogUseTex(bool useTex) {
        Get().fogUBOStruct.useTex = useTex;
    }

    FogUBOStructure RenderSystem::GetFogSettings() {
        return Get().fogUBOStruct;
    }

} // BiBuild