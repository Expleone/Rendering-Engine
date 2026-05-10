//
// Created by expleoene on 3/26/26.
//

#ifndef VIEWER_INPUTHANDLER_H
#define VIEWER_INPUTHANDLER_H
#include <map>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <unordered_map>

#include "SceneObject.h"
#include "glm/vec2.hpp"

namespace holubiho {

    enum class InputDevice {
        KEYBOARD,
        MOUSE
    };

    struct InputToken {
        InputDevice device;
        int code;
    };

    struct KeyBind {
        std::vector<InputToken> inputs;
    };

    class InputManager {

    public:

        static void Init(GLFWwindow* window);
        static void RegisterAction(std::string& name, KeyBind& bind);
        static bool IsKeyPressed(int key);
        static bool IsMouseKeyPressed(int key);
        static bool IsMouseCaptured() { return Get().isMouseCaptured; }
        static SceneObject* ObjectUnderMouse();
        static bool IsActionActive(std::string name);
        static bool SetInputMode(int mode, int value) {
            if (Get().window) {
                glfwSetInputMode(Get().window, mode, value);
                if (mode == GLFW_CURSOR) Get().mouseMode = value;
                return true;
            }
            return false;
        }
        static int GetMouseMode();
        static glm::vec2 GetMousePos();
        static glm::vec2 GetPreviousMousePos();
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;


    private:
        std::unordered_map<int, bool> keys;
        std::unordered_map<int, bool> mouseKeys;
        std::map<std::string, KeyBind> keyBinds;
        int mouseMode;
        bool isMouseCaptured{};
        GLFWwindow* window{}; // Store the window reference for cursor mode changes
        glm::vec2 mouseMovement{}; // Store mouse movement delta for camera control, etc.
        glm::vec2 currentMousePos{}; // Current mouse position
        glm::vec2 lastMousePos{}; // Last mouse position to calculate movement delta
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);



        InputManager() = default;
        ~InputManager() = default;

        static InputManager& Get();
    };
}



#endif //VIEWER_INPUTHANDLER_H