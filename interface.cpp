#include <GLFW/glfw3.h>
#include <iostream>

class GUInterface {
    private:
        double mouseClickPosX = 0;
        double mouseClickPosY = 0;
        
        GUInterface() {};

    public:
        static GUInterface& getInstance() {
            static GUInterface instance;
            return instance;
        }

        double getClickPosX() const { return mouseClickPosX; }
        double getClickPosY() const { return mouseClickPosY; }

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
            void* ptr = glfwGetWindowUserPointer(window);
            GUInterface* handler = static_cast<GUInterface*>(ptr);
            
            if (handler) {
                handler->mouse_click_handler(window, button, action, mods);
            }
        }

        void mouse_click_handler(GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                glfwGetCursorPos(window, &mouseClickPosX, &mouseClickPosY);
            }
        }
};
