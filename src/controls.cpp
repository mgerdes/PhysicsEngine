#include "controls.h"

static int mouse_scroll_x_temp;
static int mouse_scroll_y_temp;

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    mouse_scroll_x_temp = xoffset;
    mouse_scroll_y_temp = yoffset;
}

Controls::Controls(GLFWwindow *window) {
    this->window = window;

    for (int i = 0; i <= GLFW_KEY_LAST; i++) {
        key_down[i] = false;
        key_clicked[i] = false;
    }

    glfwSetScrollCallback(window, scroll_callback);
}

void Controls::update() {
    double new_mouse_pos_x, new_mouse_pos_y;
    glfwGetCursorPos(window, &new_mouse_pos_x, &new_mouse_pos_y);

    mouse_delta_x = new_mouse_pos_x - mouse_pos_x;
    mouse_delta_y = new_mouse_pos_y - mouse_pos_y;

    mouse_pos_x = new_mouse_pos_x;
    mouse_pos_y = new_mouse_pos_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        left_mouse_down = true;
    }
    else {
        left_mouse_clicked = left_mouse_down;
        left_mouse_down = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        right_mouse_down = true;
    }
    else {
        right_mouse_clicked = right_mouse_down;
        right_mouse_down = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        middle_mouse_down = true;
    }
    else {
        middle_mouse_clicked = middle_mouse_down;
        middle_mouse_down = false;
    }

    if (mouse_scroll_x_temp != 0.0) {
        mouse_scroll_x = mouse_scroll_x_temp;
        mouse_scroll_x_temp = 0.0;
    }
    else {
        mouse_scroll_x = 0.0;
    }

    if (mouse_scroll_y_temp != 0.0) {
        mouse_scroll_y = mouse_scroll_y_temp;
        mouse_scroll_y_temp = 0.0;
    }
    else {
        mouse_scroll_y = 0.0;
    }

    int i;
    for (i = 48; i < 58; i++) {
        key_clicked[i] = !glfwGetKey(window, i) && key_down[i];
    }
    for (i = 65; i < 91; i++) {
        key_clicked[i] = !glfwGetKey(window, i) && key_down[i];
    }
    for (i = 262; i < 266; i++) {
        key_clicked[i] = !glfwGetKey(window, i) && key_down[i];
    }
    key_clicked[GLFW_KEY_LEFT_SHIFT] = !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && key_down[GLFW_KEY_LEFT_SHIFT];
    key_clicked[GLFW_KEY_BACKSPACE] = !glfwGetKey(window, GLFW_KEY_BACKSPACE) && key_down[GLFW_KEY_BACKSPACE];
    key_clicked[GLFW_KEY_GRAVE_ACCENT] = !glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) && key_down[GLFW_KEY_GRAVE_ACCENT];

    for (i = 48; i < 58; i++) {
        key_down[i] = glfwGetKey(window, i);
    }
    for (i = 65; i < 91; i++) {
        key_down[i] = glfwGetKey(window, i);
    }
    for (i = 262; i < 266; i++) {
        key_down[i] = glfwGetKey(window, i);
    }
    key_down[GLFW_KEY_LEFT_SHIFT] = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    key_down[GLFW_KEY_BACKSPACE] = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    key_down[GLFW_KEY_GRAVE_ACCENT] = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT);
}
