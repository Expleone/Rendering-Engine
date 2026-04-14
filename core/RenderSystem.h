//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_RENDERSYSTEM_H
#define VIEWER_RENDERSYSTEM_H
#include <iostream>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ResourceManager.h"
#include "../helper_functions/helper.h"
#include "../components/ModelComponent.h"
#include  "SceneObject.h"
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "../components/CameraComponent.h"

namespace BiBuild {
    class SceneObject;
    class ModelComponent;

    struct LightData {
        // relative to camera position + w component determines light type (0 for directional, 1 for point, 2 for spotlight)
        glm::vec4 position;
        glm::vec4 ambient; // + w component can be used for directional/spot light direction x
        glm::vec4 diffuse; // + w component can be used for directional/spot light direction y
        glm::vec4 specular; // + w component can be used for directional/spot light direction z
        glm::vec4 attenuation; // x = constant, y = linear, z = quadratic, w = cutoff (for spotlights)
    } ;

    struct LightsUBOStructure {
        LightData lights[100];
        int numLights;
    };

    class RenderSystem {
    private:
        // Internal state that persists across frames
        // unsigned int defaultShaderProgram = 0;
        ShaderProgram* defaultShader = nullptr;
        UniformBuffer* matricesUBO = nullptr; // UBO for view and projection matrices
        UniformBuffer* lightsUBO = nullptr; // UBO for lighting data
        CameraComponent* camera = nullptr;
        LightsUBOStructure lightsUBOStruct{};


        // unsigned int gridVAO = 0, gridVBO = 0; // For drawing the 3D floor grid
        int screenWidth = 0, screenHeight = 0;

        static RenderSystem& Get() {
            static RenderSystem instance;
            return instance;
        }
        RenderSystem() = default;
        ~RenderSystem(); // Clean up shaders and global buffers here

        void OnWindowResize(GLFWwindow *window, int width, int height);

    public:

        int drawCallsLastFrame = 0; // For performance monitoring
        int objectsOnScreenLastFrame = 0;
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;

        static void Initialize(GLFWwindow *window, int width, int height, SceneObject *cameraObject);

        static void UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static ShaderProgram* GetDefaultShader();
    };

} // BiBuild

#endif //VIEWER_RENDERSYSTEM_H
