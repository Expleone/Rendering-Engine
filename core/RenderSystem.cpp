//
// Created by expleoene on 3/30/26.
//

#include "RenderSystem.h"

#include "Mesh.h"
#include "../components/LightComponent.h"
#include <uuid.h>
#include <array>
#include <cstring>

#include "InputHandler.h"


namespace holubiho {
    class SkyboxScript;

    void RenderSystem::Initialize(int width, int height, const char* winTitle, SceneObject* cameraObject) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);

        glfwWindowHint(GLFW_DEPTH_BITS, 24);

        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_SAMPLES, 10);
        Get().window = glfwCreateWindow(width, height, winTitle, nullptr, nullptr);
        if (!Get().window) {
            glfwTerminate();
            std::cerr << "Failed to create GLFW window" << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(Get().window);
        glfwSwapInterval(1); //
        Get().screenWidth = width;
        Get().screenHeight = height;
        Get().camera = cameraObject ? cameraObject->GetComponent<CameraComponent>() : nullptr;
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }
        glfwSetWindowUserPointer(Get().window, &Get());
        glfwSetFramebufferSizeCallback(Get().window, [](GLFWwindow* win, int w, int h) {
            if (auto* self = static_cast<RenderSystem*>(glfwGetWindowUserPointer(win))) {
                self->OnWindowResize(win, w, h);
            }
        });
        if (Get().camera) {
            Get().camera->aspectRatio = static_cast<float>(Get().screenWidth) / static_cast<float>(Get().screenHeight);
        }


        glEnable(GL_MULTISAMPLE);

        glViewport(0, 0, Get().screenWidth, Get().screenHeight);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        // glEnable(GL_FRAMEBUFFER_SRGB);
        glCullFace(GL_BACK);

        glFrontFace(GL_CCW);

        // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        holubiho::InputManager::Init(holubiho::RenderSystem::GetGLFWWindow());

        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(Get().window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        Get().defaultShader = ResourceManager::LoadShaderProgram("default", "./shaders/vertex/base.vert", "./shaders/fragment/base.frag");
        Get().uuidShader = ResourceManager::LoadShaderProgram("uuid", "./shaders/vertex/uuid.vert", "./shaders/fragment/uuid.frag");
        Get().matricesUBO = new UniformBuffer(sizeof(glm::mat4) * 2, static_cast<GLuint>(UBOBinding::Matrices));
        Get().lightsUBO = new UniformBuffer(sizeof(LightsUBOStructure), static_cast<GLuint>(UBOBinding::Lights));
        Get().fogUBO = new UniformBuffer(sizeof(FogUBOStructure), static_cast<GLuint>(UBOBinding::Fog));
        Get().uuidFBO = new FrameBuffer(width, height, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
    }

    RenderSystem::~RenderSystem() = default;


    void RenderSystem::UpdateAndDraw(SceneManager& scene, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        // std::vector<std::unique_ptr<SceneObject>> sceneObjects = scene.objects;
        Get().currentScene = &scene;
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
            if (!obj || obj->name == "Skybox" || !obj->render) continue;

            std::vector<ModelComponent*> models = obj->GetAllComponents<ModelComponent>();

            for (auto& model_comp : models) {
                if (!model_comp || !model_comp->mesh || !model_comp->draw) continue;

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
        }

    }


    void RenderSystem::DrawIDs(const SceneManager& scene, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        // std::vector<std::unique_ptr<SceneObject>> sceneObjects = scene.objects;


        if (!Get().uuidShader || !Get().uuidFBO) {
            std::cerr << "Error: UUIDs can't be drawn" << std::endl;
            return;
        }

        Get().uuidFBO->Bind();
        Get().uuidShader->Use();
        GLuint clearColor[4] = {0, 0, 0, 0};
        glClearBufferuiv(GL_COLOR, 0, clearColor);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_MULTISAMPLE);

        struct UniformBlock {
            glm::mat4 view;
            glm::mat4 projection;
        } uboData{ viewMatrix, projectionMatrix };

        Get().matricesUBO->UpdateSubData(&uboData, sizeof(uboData), 0);

        for (const std::unique_ptr<SceneObject>& objPtr : scene.objects) {
            SceneObject* obj = objPtr.get();
            if (!obj || obj->name == "Skybox" || !obj->render) continue;
            uint32_t uuid_parts[4];
            auto bytes = obj->uuid.as_bytes();
            std::memcpy(uuid_parts, bytes.data(), 16);
            std::vector<ModelComponent*> models = obj->GetAllComponents<ModelComponent>();

            for (auto& model_comp : models) {
                if (!model_comp || !model_comp->mesh || !model_comp->drawUUID) continue;

                Get().uuidShader->SetUniformVec4ui("uuid", uuid_parts[0], uuid_parts[1], uuid_parts[2], uuid_parts[3]);
                model_comp->Draw(Get().uuidShader);
            }
        }
        Get().uuidFBO->Unbind();
        glEnable(GL_MULTISAMPLE);
    }

    SceneObject *RenderSystem::GetObjectFromScreen(int mx, int my) {
        if (!Get().currentScene) {
            return nullptr;
        }

        my = Get().screenHeight - my;
        Get().uuidFBO->Bind();
        uint32_t read_parts[4];

        glReadPixels(mx, my, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, read_parts);

        std::array<unsigned char, 16> reconstructed_bytes = {};
        std::memcpy(reconstructed_bytes.data(), read_parts, 16);

        uuids::uuid reconstructed_id = uuids::uuid(reconstructed_bytes.begin(), reconstructed_bytes.end());
        Get().uuidFBO->Unbind();

        return Get().currentScene->GetObject(reconstructed_id);
    }

    void RenderSystem::OnWindowResize(GLFWwindow* window, int  width, int height) {
        screenWidth = width;
        screenHeight = height;
        uuidFBO->OnResize(width, height);
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
    void RenderSystem::SetNightFogTexture(Texture *tex) {
        Get().nightFogTex = tex;
        Get().fogUBOStruct.useTex = true;
    }

    Texture *RenderSystem::GetFogTexture() {
        if (Get().fogUBOStruct.useTex) {
            return  Get().fogTex;
        }
        return nullptr;
    }
    Texture *RenderSystem::GetNightFogTexture() {
        if (Get().fogUBOStruct.useTex) {
            return  Get().nightFogTex;
        }
        return nullptr;
    }

    void RenderSystem::SetSunPosPointer(glm::vec3* pos) {
            Get().sunPosPointer = pos;
    }

    glm::vec3 RenderSystem::GetSunPosition() {
        if (Get().sunPosPointer) {
            return *Get().sunPosPointer;
        }
        return glm::vec3(0);
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

    FrameBuffer *RenderSystem::GetUUIDFrameBuffer() {
        return Get().uuidFBO;
    }


    //For debug purposes
    void RenderSystem::DrawFullscreenQuad(GLuint textureID) {
        auto shaderProgram = ResourceManager::LoadShaderProgram("debugFrameBuffer", "./shaders/vertex/debugframebuffer.vert", "./shaders/fragment/debugframebuffer.frag");
        static GLuint vao = 0;
        if (vao == 0) {
            float quadVertices[] = {
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f,  0.0f, 1.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f
            };
            GLuint vbo;
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shaderProgram->SetUniformInt("fboTexture", 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    GLFWwindow* RenderSystem::GetGLFWWindow() {
        return Get().window;
    }
} // BiBuild