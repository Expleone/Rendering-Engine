//
// Created by expleoene on 3/26/26.
//

#include "InputHandler.h"

using namespace BiBuild;

std::unordered_map<int, bool> InputManager::keys;
glm::vec2 InputManager::mouseMovement{0.0f, 0.0f};
glm::vec2 InputManager::lastMousePos{0.0f, 0.0f};
glm::vec2 InputManager::currentMousePos{0.0f, 0.0f};
bool InputManager::isMouseCaptured = false;
GLFWwindow* InputManager::window = nullptr;


void InputManager::Init(GLFWwindow* window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    InputManager::window = window; // Store the window reference for later use
}

bool InputManager::IsKeyPressed(int key) {
    auto it = keys.find(key);
    if (it != keys.end()) {
        return it->second;
    }
    return false;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key != GLFW_KEY_UNKNOWN) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    lastMousePos = currentMousePos;
    currentMousePos = glm::vec2(xpos, ypos);
    mouseMovement = currentMousePos - lastMousePos;
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Implement scroll handling if needed
}

void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_KEY_UNKNOWN) {
        if (action == GLFW_PRESS) {
            keys[button] = true;
        } else if (action == GLFW_RELEASE) {
            keys[button] = false;
        }
    }
}