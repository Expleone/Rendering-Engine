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
#include "../helper_functions/helper.h"
#include "imgui_impl_opengl3_loader.h"
#include "../components/MeshComponent.h"
#include  "SceneObject.h"
#include "../components/CameraComponent.h"

namespace BiBuild {
    class SceneObject;
    class MeshComponent;


    class RenderSystem {
    private:
        // Internal state that persists across frames
        unsigned int defaultShaderProgram = 0;
        unsigned int gridVAO = 0, gridVBO = 0; // For drawing the 3D floor grid
        int screenWidth = 0, screenHeight = 0;
        CameraComponent* camera = nullptr;

        // Private helper methods
        void CompileShaders();
        void UpdateGPUBuffers(MeshComponent* mesh);

    public:
        RenderSystem() = default;
        ~RenderSystem(); // Clean up shaders and global buffers here

        // Called once when the app starts
        void Initialize(int width, int height, SceneObject* cameraObject);

        GLuint compileShader(GLenum type, const char* source) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            int success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
            }
            return shader;
        }

        GLuint createProgram(const char* vShaderCode, const char* fShaderCode) {
            GLuint vShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
            GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

            GLuint program = glCreateProgram();
            glAttachShader(program, vShader);
            glAttachShader(program, fShader);
            glLinkProgram(program);

            int success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(program, 512, nullptr, infoLog);
                std::cerr << "Program Linking Error:\n" << infoLog << std::endl;
            }
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return program;
        }


        // Called if the user resizes the window
        void OnWindowResize(int width, int height);

        // Called every single frame
        void UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    };

} // BiBuild

#endif //VIEWER_RENDERSYSTEM_H
