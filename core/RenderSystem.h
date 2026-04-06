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
#include "../components/MeshComponent.h"
#include  "SceneObject.h"
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "../components/CameraComponent.h"

namespace BiBuild {
    class SceneObject;
    class MeshComponent;

    struct LightData {
        // relative to camera position + w component determines light type (0 for directional, 1 for point, 2 for spotlight)
        glm::vec4 position;
        glm::vec4 ambient; // + w component can be used for directional/spot light direction x
        glm::vec4 diffuse; // + w component can be used for directional/spot light direction y
        glm::vec4 specular; // + w component can be used for directional/spot light direction z
        glm::vec4 attenuation; // x = constant, y = linear, z = quadratic, w = cutoff (for spotlights)
    } ;

    inline struct LightsUBOStructure {
        LightData lights[100];
        int numLights;
    } lightData;

    class RenderSystem {
    private:
        // Internal state that persists across frames
        // unsigned int defaultShaderProgram = 0;
        ShaderProgram* defaultShader = nullptr;
        UniformBuffer* matricesUBO = nullptr; // UBO for view and projection matrices
        UniformBuffer* lightsUBO = nullptr; // UBO for lighting data
        ResourceManager* resourceManager = nullptr; // Reference to the resource manager for loading meshes and shaders
        CameraComponent* camera = nullptr;


        // unsigned int gridVAO = 0, gridVBO = 0; // For drawing the 3D floor grid
        int screenWidth = 0, screenHeight = 0;


        // Private helper methods
        void CompileShaders();
        void UpdateGPUBuffers(MeshComponent* mesh);
        void OnWindowResize(GLFWwindow *window, int width, int height);

    public:
        int drawCallsLastFrame = 0; // For performance monitoring
        int objectsOnScreenLastFrame = 0;
        RenderSystem() = default;
        ~RenderSystem(); // Clean up shaders and global buffers here

        // Called once when the app starts
        void Initialize(GLFWwindow *window, int width, int height, SceneObject *cameraObject, ResourceManager *resourceMgr);

        int RenderMeshObject(const SceneObject*obj);

        // Called every single frame
        void UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    };

} // BiBuild

#endif //VIEWER_RENDERSYSTEM_H
