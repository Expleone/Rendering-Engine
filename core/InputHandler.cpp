//
// Created by expleoene on 3/26/26.
//

#include "InputHandler.h"

#include "RenderSystem.h"

using namespace holubiho;



InputManager& InputManager::Get() {
    static InputManager it = InputManager();
    return it;
}


void InputManager::Init(GLFWwindow* window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    Get().window = window; // Store the window reference for later use
}

void InputManager::RegisterAction(std::string &name, KeyBind& bind) {
    auto it = Get().keyBinds.find(name);
    if (it != Get().keyBinds.end()) {
        Get().keyBinds[name] = bind;
        std::cout << "Keybind changed for action \"" << name << "\"." << std::endl;
        return;
    }
    Get().keyBinds.emplace(name, bind);
    std::cout << "Keybind registered for action \"" << name << "\"." << std::endl;
}

bool InputManager::IsKeyPressed(int key) {
    auto it = Get().keys.find(key);
    if (it != Get().keys.end()) {
        return it->second;
    }
    return false;
}

bool InputManager::IsMouseKeyPressed(int key) {
    auto it = Get().mouseKeys.find(key);
    if (it != Get().mouseKeys.end()) {
        return it->second;
    }
    return false;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key != GLFW_KEY_UNKNOWN) {
        if (action == GLFW_PRESS) {
            Get().keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            Get().keys[key] = false;
        }
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Get().lastMousePos = Get().currentMousePos;
    Get().currentMousePos = glm::vec2(xpos, ypos);
    Get().mouseMovement = Get().currentMousePos - Get().lastMousePos;
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Implement scroll handling if needed
}

void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_KEY_UNKNOWN) {
        if (action == GLFW_PRESS) {
            Get().mouseKeys[button] = true;
        } else if (action == GLFW_RELEASE) {
            Get().mouseKeys[button] = false;
        }
    }
}

SceneObject *InputManager::ObjectUnderMouse() {
    return RenderSystem::GetObjectFromScreen(Get().currentMousePos.x, Get().currentMousePos.y);
}

bool InputManager::IsActionActive(std::string name) {
    auto it = Get().keyBinds.find(name);
    if (it == Get().keyBinds.end()) {
        std::cout << "Warning: Keybind \"" + name + "\" not found" << std::endl;
        return false;
    }
    bool isActive = true;
    auto bind = it->second;
    for (auto& input : bind.inputs) {
        switch (input.device) {
            case InputDevice::KEYBOARD:
                if (!IsKeyPressed(input.code)) {
                    isActive = false;
                }
                break;
            case InputDevice::MOUSE:
                if (!IsMouseKeyPressed(input.code)) {
                    isActive = false;
                }
        }
        if (!isActive) break;
    }
    return isActive;
}

int InputManager::GetMouseMode() {
    return Get().mouseMode;
}

glm::vec2 InputManager::GetMousePos() {
    return Get().currentMousePos;
}

glm::vec2 InputManager::GetPreviousMousePos() {
    return Get().lastMousePos;
}