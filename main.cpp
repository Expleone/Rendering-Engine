#include <iostream>
#include <vector>

// Include GLAD before GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "test_models/birds.h"

const int WIN_WIDTH  = 800;
const int WIN_HEIGHT = 600;
const char* WIN_TITLE  = "GLFW & ImGui - Shaders data";

const int animFrameTimeMs = 150;

int draw_types[] = {
  GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY,
  GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES,
  GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_PATCHES
};

const char* draw_types_str[] = {
  "GL_POINTS", "GL_LINE_STRIP", "GL_LINE_LOOP", "GL_LINES", "GL_LINE_STRIP_ADJACENCY",
  "GL_LINES_ADJACENCY", "GL_TRIANGLE_STRIP", "GL_TRIANGLE_FAN", "GL_TRIANGLES",
  "GL_TRIANGLE_STRIP_ADJACENCY", "GL_TRIANGLES_ADJACENCY", "GL_PATCHES"
};

struct State {
  glm::mat4 projection;
  glm::mat4 model;
  int frame, nextFrame;
  float t;
  float yaw = 0.0f;
  float pitch = 0.0f;
  float scale = 1.0f;
  int draw_type = 8; // Default to GL_TRIANGLES
  bool isDragging = false;
  double lastMouseX = 0.0, lastMouseY = 0.0;
} state;

struct Resources {
  GLuint program;
  GLuint vbo, ebo;
  GLuint vao;
} resources;

struct Locations {
  GLint position;
  GLint NextPositions;
  GLint PVM;
  GLint scale;
  GLint t;
  GLint color;
} handles;

const char * srcVertexShader = R"(
  #version 330 core
  in vec3 aPosition;
  in vec3 aNextPositions;
  uniform mat4 PVM;
  uniform float scale;
  uniform float t;

  void main() {
    vec3 pos = mix(aPosition, aNextPositions, t);
    pos = scale * pos;
    gl_Position = PVM * vec4(pos, 1.0f);
  }
)";

const char * srcFragmentShader = R"(
  #version 330 core
  uniform vec3 color;
  out vec4 fragmentColor;

  void main() {
    fragmentColor = vec4(color, 1.0f);
  }
)";

// --- Helper Functions to replace pgr framework ---
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

// --- Callbacks ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (height == 0) height = 1;
    state.projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 1.0f, 20.0f);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (yoffset > 0) state.scale *= 1.1f;
    else if (yoffset < 0) state.scale /= 1.1f;

    if (state.scale > 2.0f) state.scale = 2.0f;
    if (state.scale < 0.1f) state.scale = 0.1f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            state.isDragging = true;
            glfwGetCursorPos(window, &state.lastMouseX, &state.lastMouseY);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            state.isDragging = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (state.isDragging) {
        double dx = xpos - state.lastMouseX;
        double dy = ypos - state.lastMouseY;
        state.lastMouseX = xpos;
        state.lastMouseY = ypos;

        state.yaw += (float)dx * 0.5f;
        state.pitch += (float)dy * 0.5f;
    }
}

bool initOpenGL() {
    resources.program = createProgram(srcVertexShader, srcFragmentShader);
    if (resources.program == 0) return false;

    handles.position = glGetAttribLocation(resources.program, "aPosition");
    handles.NextPositions = glGetAttribLocation(resources.program, "aNextPositions");
    handles.PVM = glGetUniformLocation(resources.program, "PVM");
    handles.t = glGetUniformLocation(resources.program, "t");
    handles.color = glGetUniformLocation(resources.program, "color");
    handles.scale = glGetUniformLocation(resources.program, "scale");

    glGenVertexArrays(1, &resources.vao);
    glBindVertexArray(resources.vao);

    glGenBuffers(1, &resources.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, resources.vbo);
    glBufferData(GL_ARRAY_BUFFER, birds_data.nVertices * 3 * birds_data.nAnimFrames * sizeof(float), birds_data.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &resources.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, birds_data.nFaces * 3 * sizeof(GLushort), birds_data.faces, GL_STATIC_DRAW);

    glVertexAttribPointer(handles.position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(handles.position);

    glVertexAttribPointer(handles.NextPositions, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(handles.NextPositions);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.4f, 0.8f, 1.0f);

    return true;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Callbacks setup
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (!initOpenGL()) return -1;

    // Trigger initial projection setup
    framebuffer_size_callback(window, WIN_WIDTH, WIN_HEIGHT);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Animation timer logic
        double timeNow = glfwGetTime() * 1000.0;
        int timeMs = (int)timeNow;
        int e = timeMs % animFrameTimeMs;
        state.frame = (timeMs / animFrameTimeMs) % birds_data.nAnimFrames;
        state.nextFrame = (state.frame + 1) % birds_data.nAnimFrames;
        state.t = float(e) / float(animFrameTimeMs);

        // if (state.isDragging) {
        //     int width, height;
        //     glfwGetWindowSize(window, &width, &height);
        //     int xpos = width / 2.0;
        //     int ypos = height / 2.0;
        //     glfwSetCursorPos(window, xpos, ypos);
        //     state.lastMouseX = xpos;
        //     state.lastMouseY = ypos;
        // }

        // Update Matrix
        state.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f)) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(state.pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(state.yaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
                      glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui Window
        ImGui::Begin("Controls");
        ImGui::SliderFloat("Scale", &state.scale, 0.1f, 2.0f);
        ImGui::Combo("Draw Type", &state.draw_type, draw_types_str, IM_ARRAYSIZE(draw_types_str));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // Render OpenGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(resources.program);

        glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(state.projection * state.model));
        glUniform1f(handles.t, state.t);
        glUniform3fv(handles.color, 1, birds_data.color);
        glUniform1f(handles.scale, state.scale);

        glBindVertexArray(resources.vao);
        glBindBuffer(GL_ARRAY_BUFFER, resources.vbo);

        size_t frameOffset = state.frame * birds_data.nVertices * 3 * sizeof(float);
        glVertexAttribPointer(handles.position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)frameOffset);

        size_t nextFrameOffset = state.nextFrame * birds_data.nVertices * 3 * sizeof(float);
        glVertexAttribPointer(handles.NextPositions, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nextFrameOffset);

        glDrawElements(draw_types[state.draw_type], birds_data.nFaces * 3, GL_UNSIGNED_SHORT, (void*)0);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &resources.vao);
    glDeleteBuffers(1, &resources.vbo);
    glDeleteBuffers(1, &resources.ebo);
    glDeleteProgram(resources.program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}