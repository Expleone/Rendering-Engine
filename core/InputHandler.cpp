//
// Created by expleoene on 3/26/26.
//

#include "InputHandler.h"

using namespace BiBuild;

std::unordered_map<int, bool> InputManager::keys;

void InputManager::Init(GLFWwindow* window) {
    glfwSetKeyCallback(window, KeyCallback);
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