#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "scene.h"
#include "shader.h"

GLFWwindow *init_opengl(int width, int height, const char *title) {
    if (!glfwInit()) {
        // TODO: Log error
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        // TODO: Log error 
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        // TODO: Log error 
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return window;
};

int main() {
    GLFWwindow *window = init_opengl(500, 500, "hi");
    int window_width, window_height;

    Scene scene;
    scene.camera.eye = vec3(1.0, 1.0, 1.0);
    scene.camera.target = vec3(0.0, 0.0, 0.0);
    scene.camera.up = vec3(0.0, 1.0, 0.0);
    scene.camera.fov = 67.0;
    scene.camera.aspect = 1.0;
    scene.camera.near = 0.1;
    scene.camera.far = 100.0;

    std::vector<int> mesh_ids;
    scene.add_meshes_from_file("resources/woodPlank.obj", &mesh_ids);

    int instance_id = scene.add_instance(mesh_ids[0]);
    int transform_id = scene.instances[instance_id].transform_id;
    scene.transforms[transform_id].translation = vec3(0.0, 0.0, 0.0);
    scene.transforms[transform_id].scale = vec3(0.2, 0.4, 0.1);

    instance_id = scene.add_instance(mesh_ids[0]);
    transform_id = scene.instances[instance_id].transform_id;
    scene.transforms[transform_id].translation = vec3(0.0, -1.0, -1.0);
    scene.transforms[transform_id].scale = vec3(0.2, 0.4, 0.1);

    Renderer renderer;
    renderer.scene = &scene;
    renderer.shader = Shader::load_from_file("resources/default.frag", "resources/default.vert");
    
    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &window_width, &window_height);

        renderer.resize(window_width, window_height);
        renderer.paint();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
};
