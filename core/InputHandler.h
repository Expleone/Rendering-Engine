//
// Created by expleoene on 3/26/26.
//

#ifndef VIEWER_INPUTHANDLER_H
#define VIEWER_INPUTHANDLER_H
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace BiBuild {
    class InputManager {
    public:
        static void Init(GLFWwindow* window);
        static bool IsKeyPressed(int key);

    private:
        static std::unordered_map<int, bool> keys;
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}



#endif //VIEWER_INPUTHANDLER_H