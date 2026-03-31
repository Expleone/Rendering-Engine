//
// Created by expleoene on 3/26/26.
//

#ifndef VIEWER_INPUTHANDLER_H
#define VIEWER_INPUTHANDLER_H
#include <GLFW/glfw3.h>
#include <unordered_map>

#include "glm/vec2.hpp"

namespace BiBuild {
    class InputManager {
    public:
        static void Init(GLFWwindow* window);
        static bool IsKeyPressed(int key);
        static bool IsMouseCaptured() { return isMouseCaptured; }
        static bool SetInputMode(int mode, int value) {
            if (window) {
                glfwSetInputMode(window, mode, value);
                return true;
            }
            return false;
        }
        static glm::vec2 mouseMovement; // Store mouse movement delta for camera control, etc.
        static glm::vec2 currentMousePos; // Current mouse position
        static glm::vec2 lastMousePos; // Last mouse position to calculate movement delta

    private:
        static std::unordered_map<int, bool> keys;
        static bool isMouseCaptured; // Track if the mouse is currently captured for camera control
        static GLFWwindow* window; // Store the window reference for cursor mode changes
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    };
}



#endif //VIEWER_INPUTHANDLER_H