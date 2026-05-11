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
    /// @brief Singleton class that manages user input for the application.
    class InputManager {

    public:
        /// @brief Initializes the input manager with the given GLFW window.
        /// @param window The GLFW window to which the input manager will attach its callbacks for handling keyboard and mouse input.
        static void Init(GLFWwindow* window);
        /// @brief Registers a new action with the specified name and key bindings.
        /// @param name The name of the action to register. This name will be used to query the state of the action later.
        /// @param bind The KeyBind struct that defines the input tokens (keyboard keys and mouse buttons) that will trigger the action when pressed. Each InputToken specifies the device type (keyboard or
        static void RegisterAction(std::string& name, KeyBind& bind);
        /// @brief Checks if a specific keyboard key is currently pressed.
        /// @param key The GLFW key code of the keyboard key to check (e.g., GLFW_KEY_W for the 'W' key).
        static bool IsKeyPressed(int key);

        /// @brief Checks if a specific mouse button is currently pressed.
        /// @param key The GLFW mouse button code to check (e.g., GLFW_MOUSE_BUTTON_LEFT for the left mouse button).
        static bool IsMouseKeyPressed(int key);
        /// @brief Checks if the mouse is currently captured (i.e., if the cursor is hidden and locked to the window).
        static bool IsMouseCaptured() { return Get().isMouseCaptured; }

        /// @brief Returns a pointer to the SceneObject that is currently under the mouse cursor
        static SceneObject* ObjectUnderMouse();

        /// @brief Checks if the specified action is currently active.
        /// @param name The name of the action to check, which should have been registered previously using RegisterAction.
        static bool IsActionActive(std::string name);
        static bool SetInputMode(int mode, int value) {
            if (Get().window) {
                glfwSetInputMode(Get().window, mode, value);
                if (mode == GLFW_CURSOR) Get().mouseMode = value;
                return true;
            }
            return false;
        }
        /// @brief Returns the current mouse mode (e.g., GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, or GLFW_CURSOR_DISABLED) that indicates how the mouse cursor is being handled in the application.
        static int GetMouseMode();

        /// @brief Returns the current position of the mouse cursor in screen coordinates (pixels) relative to the top-left corner of the window. The returned glm::vec2 contains the x and y coordinates of the mouse cursor.
        static glm::vec2 GetMousePos();

        /// @brief Returns position of the mouse cursor in previous frame.
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