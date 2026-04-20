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

    struct FogUBOStructure {
        glm::vec4 color;
        float distClose;
        float distFar;
        int useTex;
    };

    class RenderSystem {
    private:
        // Internal state that persists across frames
        // unsigned int defaultShaderProgram = 0;
        ShaderProgram* defaultShader = nullptr;
        UniformBuffer* matricesUBO = nullptr; // UBO for view and projection matrices
        UniformBuffer* lightsUBO = nullptr; // UBO for lighting data
        UniformBuffer* fogUBO = nullptr;
        CameraComponent* camera = nullptr;
        LightsUBOStructure lightsUBOStruct{};
        bool useSkyboxTexAsFog = true;
        FogUBOStructure fogUBOStruct{glm::vec4(0), 100, 300,  0};
        Texture* fogTex = nullptr;

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

        static void UpdateAndDraw(const SceneManager &scene, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
        static ShaderProgram* GetDefaultShader();
        static void SetDefaultShader(ShaderProgram* prg);
        static void SetFogTexture(Texture* tex);
        static Texture* GetFogTexture();
        static void SetUseSkyboxTexAsFog(bool use);
        static bool GetUseSkyboxTexAsFog();
        static void SetFogSettings(FogUBOStructure settings);
        static void SetFogSettings(glm::vec3 color, float distanceClose, float distanceFar, bool useTex);
        static void SetFogColor(glm::vec3 color);
        static void SetFogDistance(float distanceClose, float distanceFar);
        static void SetFogUseTex(bool useTex);
        static FogUBOStructure GetFogSettings();
    };

} // BiBuild

#endif //VIEWER_RENDERSYSTEM_H
