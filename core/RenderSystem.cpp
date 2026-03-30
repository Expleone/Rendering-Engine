//
// Created by expleoene on 3/30/26.
//

#include "RenderSystem.h"


namespace BiBuild {
    struct Locations {
        GLint position;
        GLint NextPositions;
        GLint PVM;
        GLint scale;
        GLint t;
        GLint color;
    } handles;

    void RenderSystem::Initialize(int width, int height, SceneObject* cameraObject) {
        screenWidth = width;
        screenHeight = height;
        camera = cameraObject ? cameraObject->GetComponent<CameraComponent>() : nullptr;
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

        // Set the default background color (e.g., a dark gray editor background)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        // 3. Compile base assets
        // Call the internal helper to compile the vertex and fragment shaders
        CompileShaders();


        // 4. Initialize persistent editor visuals (Optional)
        // If you have a 3D floor grid, you would generate its VAO/VBO here.
        // SetupFloorGrid();
    }

    RenderSystem::~RenderSystem() {
        if (defaultShaderProgram != 0) {
            glDeleteProgram(defaultShaderProgram);
            defaultShaderProgram = 0;
        }

        if (gridVBO != 0) {
            glDeleteBuffers(1, &gridVBO);
            gridVBO = 0;
        }

        if (gridVAO != 0) {
            glDeleteVertexArrays(1, &gridVAO);
            gridVAO = 0;
        }
    }

    void RenderSystem::CompileShaders() {
        std::string vertStr;
        std::string fragStr;

        vertStr = BiBuild::Helper::read_file("./shaders/vertex/base.vert");
        fragStr = BiBuild::Helper::read_file("./shaders/fragment/base.frag");

        defaultShaderProgram = createProgram(vertStr.c_str(), fragStr.c_str());
        handles.position = glGetAttribLocation(defaultShaderProgram, "aPosition");
        // handles.NextPositions = glGetAttribLocation(defaultShaderProgram, "aNextPositions");
        handles.PVM = glGetUniformLocation(defaultShaderProgram, "PVM");
        // handles.t = glGetUniformLocation(defaultShaderProgram, "t");
        handles.color = glGetUniformLocation(defaultShaderProgram, "color");
        // handles.scale = glGetUniformLocation(defaultShaderProgram, "scale");
    }

    void RenderSystem::UpdateAndDraw(const std::vector<std::unique_ptr<SceneObject>>& sceneObjects, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(defaultShaderProgram);
        for (const std::unique_ptr<SceneObject>& objPtr : sceneObjects) {
            SceneObject* obj = objPtr.get();
            auto* mesh = obj->GetComponent<MeshComponent>();

            if (!mesh) continue;

            // 1. Check if the GPU needs updated data
            if (mesh->isDirty) {
                UpdateGPUBuffers(mesh);
                mesh->isDirty = false; // Reset the flag
            }
            
            auto modelMatrix = obj->transform->worldMatrix;
            // 2. Standard Drawing Code
            glBindVertexArray(mesh->VAO);
            glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(projectionMatrix*viewMatrix*modelMatrix));
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
        }
    }

    void RenderSystem::UpdateGPUBuffers(MeshComponent* mesh) {
        // If VAO doesn't exist yet, generate it
        if (mesh->VAO == 0) {
            glGenVertexArrays(1, &mesh->VAO);
            glGenBuffers(1, &mesh->VBO);
            glGenBuffers(1, &mesh->EBO);
        }

        glBindVertexArray(mesh->VAO);

        // Upload Vertex Data
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mesh->vertices.size() * sizeof(Vertex)), mesh->vertices.data(), GL_STATIC_DRAW);

        // Upload Index Data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mesh->indices.size() * sizeof(unsigned int)), mesh->indices.data(), GL_STATIC_DRAW);

        // ... set vertex attribute pointers ...
        glVertexAttribPointer(handles.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) - sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0);
    }
} // BiBuild