#pragma once

#include <GLFW/glfw3.h>

class Controls {
    private:
        GLFWwindow *window;

    public:
        Controls(GLFWwindow *window);

        bool left_mouse_down, left_mouse_clicked;
        bool right_mouse_down, right_mouse_clicked;
        bool middle_mouse_down, middle_mouse_clicked;

        float mouse_pos_x, mouse_delta_x, mouse_pos_y, mouse_delta_y;

        bool key_down[GLFW_KEY_LAST+1];
        bool key_clicked[GLFW_KEY_LAST+1];

        void update();
};
