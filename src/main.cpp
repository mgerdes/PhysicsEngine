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
    GLFWwindow *window = init_opengl(700, 700, "hi");
    Controls controls(window);
    int window_width, window_height;

    Scene scene;
    scene.camera.eye = vec3(0.0, 10.0, 10.0);
    scene.camera.target = vec3(0.0, 0.0, 0.0);
    scene.camera.up = vec3(0.0, 1.0, 0.0);
    scene.camera.fov = 67.0;
    scene.camera.aspect = 1.0;
    scene.camera.near = 0.1;
    scene.camera.far = 100.0;

    std::vector<int> mesh_ids;
    scene.add_meshes_from_file("resources/cube.obj", &mesh_ids);

    int instance_id = scene.add_instance(mesh_ids[0]);
    int transform_id = scene.instances[instance_id].transform_id;
    scene.transforms[transform_id].translation = vec3(0.0, 0.0, 0.0);
    scene.transforms[transform_id].scale = vec3(1.0, 1.0, 1.0);

    int instance_id_2 = scene.add_instance(mesh_ids[0]);
    int transform_id_2 = scene.instances[instance_id_2].transform_id;
    scene.transforms[transform_id_2].translation = vec3(0.0, -5.0, 0.0);
    scene.transforms[transform_id_2].scale = vec3(10.0, 10.0, 10.0);

    Renderer renderer;
    renderer.scene = &scene;
    renderer.shader = Shader::load_from_file("shaders/default.frag", "shaders/default.vert");

    RigidBody cube_rigid_body;
    cube_rigid_body.mass = 1.0;
    cube_rigid_body.inertia_tensor = mat4(
            (1.0 / 12.0) * 2, 0.0,              0.0,              0.0,
            0.0,              (1.0 / 12.0) * 2, 0.0,              0.0,
            0.0,              0.0,              (1.0 / 12.0) * 2, 0.0,
            0.0,              0.0,              0.0,              1.0
            );
    cube_rigid_body.position = vec3(0.0, 5.0, 0.0);
    cube_rigid_body.add_force_at_point(vec3(10.0, 0.0, 9.0), vec3(0.0, 7.0, 0.0));
    cube_rigid_body.transform_id = instance_id;

    PhysicsEngine physics_engine;
    physics_engine.add_rigid_body(cube_rigid_body);

    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &window_width, &window_height);
        controls.update();

        if (controls.key_down[GLFW_KEY_R]) {
            physics_engine.rigid_bodies[0].position = vec3(0.0, 5.0, 0.0);
        }

        physics_engine.run_physics(0.016);
        for (int i = 0; i < physics_engine.rigid_bodies.size(); i++) {
            RigidBody body = physics_engine.rigid_bodies[i];
            if (body.transform_id != -1) {
                scene.transforms[body.transform_id].translation = body.position;
                scene.transforms[body.transform_id].orientation = body.orientation;
            }
        }

        renderer.resize(window_width, window_height);
        renderer.paint();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
};
