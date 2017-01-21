#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "physics_engine.h"
#include "controls.h"

GLFWwindow *init_opengl(int width, int height, const char *title) {
    if (!glfwInit()) {
        // TODO: Log error
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    glEnable(GL_MULTISAMPLE);  

    return window;
};

int main() {
    GLFWwindow *window = init_opengl(1000, 1000, "hi");
    Controls controls(window);
    int window_width, window_height;
    std::vector<int> cube_mesh_ids;
    std::vector<int> plane_mesh_ids;

    Scene scene;
    scene.camera.eye = vec3(0.0, 3.0, 10.0);
    scene.camera.target = vec3(0.0, 3.0, 0.0);
    scene.camera.up = vec3(0.0, 1.0, 0.0);
    scene.camera.fov = 67.0;
    scene.camera.aspect = 1.0;
    scene.camera.near = 0.1;
    scene.camera.far = 100.0;
    scene.add_meshes_from_file("resources/cube.obj", &cube_mesh_ids);
    scene.add_meshes_from_file("resources/plane.obj", &plane_mesh_ids);

    Renderer renderer;
    renderer.scene = &scene;
    renderer.shader = Shader::load_from_file("shaders/default.frag", "shaders/default.vert");

    PhysicsEngine physics_engine;
    physics_engine.scene = &scene;

    int plane_instance_id, plane_transform_id;
    int cube_instance_id, cube_transform_id;

    plane_instance_id = scene.add_instance(plane_mesh_ids[0]);
    plane_transform_id = scene.instances[plane_instance_id].transform_id;
    physics_engine.add_plane_collider(plane_transform_id);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(1.8, 0.1, 0.5), vec3(0.2, 2.5, 0.0), quat(), 1.0);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.0, 0.5, 0.0), quat(), 1.0);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(0.2, 0.2, 0.2), vec3(1.2, 4.5, 0.0), quat(), 0.1);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.2, 3.5, 0.0), quat(), 1.0);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(-1.2, 20.0, 0.0), quat(), 1.0);

    cube_instance_id = scene.add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene.instances[cube_instance_id].transform_id;
    physics_engine.add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.0, 50.0, 0.0), quat(), 10.0);

    physics_engine.update(0.0);

    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &window_width, &window_height);
        controls.update();

        if (controls.key_down[GLFW_KEY_P] || controls.key_clicked[GLFW_KEY_O]) {
            physics_engine.update(0.016);
        }

        renderer.resize(window_width, window_height);
        renderer.paint();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
};
