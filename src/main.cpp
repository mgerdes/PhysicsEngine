#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "physics_engine.h"
#include "controls.h"

static std::vector<int> cube_mesh_ids;
static std::vector<int> plane_mesh_ids;
Collider *controlled_cube;

GLFWwindow *init_opengl(int width, int height, const char *title) {
    if (!glfwInit()) {
        // TODO: Log error
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
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
    glEnable(GL_LINE_SMOOTH);

    return window;
}

void init_catapult_scene(PhysicsEngine *physics_engine) {
    int plane_instance_id, plane_transform_id;
    int cube_instance_id, cube_transform_id;
    Scene *scene = physics_engine->scene;

    plane_instance_id = scene->add_instance(plane_mesh_ids[0]);
    scene->instances[plane_instance_id].casts_shadow = false;
    scene->instances[plane_instance_id].draw_outline = false;
    plane_transform_id = scene->instances[plane_instance_id].transform_id;
    physics_engine->add_plane_collider(plane_transform_id);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(1.8, 0.1, 0.5), vec3(0.3, 2.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.0, 0.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.2, 0.2, 0.2), vec3(1.2, 4.5, 0.0), quat(), 0.1, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.2, 3.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(-1.2, 20.0, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.55, 50.0, 0.0), quat(), 10.0, 0.2, 0.4, 0.3, false);

    physics_engine->update(0.0);
}

void init_wall_scene(PhysicsEngine *physics_engine) {
    int plane_instance_id, plane_transform_id;
    int cube_instance_id, cube_transform_id;
    int collider_id;
    Scene *scene = physics_engine->scene;

    plane_instance_id = scene->add_instance(plane_mesh_ids[0]);
    scene->instances[plane_instance_id].casts_shadow = false;
    scene->instances[plane_instance_id].draw_outline = false;
    plane_transform_id = scene->instances[plane_instance_id].transform_id;
    physics_engine->add_plane_collider(plane_transform_id);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(0.0, 0.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.0, 0.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(-1.0, 0.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(0.0, 1.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(1.0, 1.5, 0.0), quat(), 1.00, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(-1.0, 1.5, 0.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(2.0, 2.0, 0.1), vec3(0.0, 5.0, -5.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(0.2, 0.2, 0.2), vec3(0.0, 6.5, -4.6), quat(), 0.12, 0.2, 0.4, 0.3, false);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    collider_id = physics_engine->add_cube_collider(cube_transform_id, vec3(0.5, 0.5, 0.5), vec3(0.0, 1.4, 5.0), quat(), 1.0, 0.2, 0.4, 0.3, false);

    Collider *collider = physics_engine->colliders[collider_id];
    RigidBody *body = &collider->body;
    body->add_force_at_point(vec3(0.0, 0.0, -2150.0), body->position);

    physics_engine->update(0.016);
}

void init_jump_scene(PhysicsEngine *physics_engine) {
    int plane_instance_id, plane_transform_id;
    int cube_instance_id, cube_transform_id;
    int collider_id;
    Scene *scene = physics_engine->scene;

    plane_instance_id = scene->add_instance(plane_mesh_ids[0]);
    scene->instances[plane_instance_id].casts_shadow = false;
    scene->instances[plane_instance_id].draw_outline = false;
    plane_transform_id = scene->instances[plane_instance_id].transform_id;
    physics_engine->add_plane_collider(plane_transform_id);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(1.0, 0.7, 1.0), vec3(-4.0, 0.7, 0.0), quat(vec3(0.0, 1.0, 0.0), 0.75), 1.0, 0.2, 0.4, 0.3, true);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(1.0, 1.4, 1.0), vec3(-1.0, 1.4, 0.0), quat(vec3(0.0, 1.0, 0.0), 0.55), 1.0, 0.2, 0.4, 0.3, true);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(1.0, 2.1, 1.0), vec3(2.0, 2.1, -1.0), quat(vec3(0.0, 1.0, 0.0), 0.25), 1.0, 0.2, 0.4, 0.3, true);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    physics_engine->add_cube_collider(cube_transform_id, vec3(1.0, 2.8, 1.0), vec3(5.0, 2.8, -2.0), quat(vec3(0.0, 1.0, 0.0), 0.83), 1.0, 0.2, 0.4, 0.3, true);

    cube_instance_id = scene->add_instance(cube_mesh_ids[0]);
    cube_transform_id = scene->instances[cube_instance_id].transform_id;
    collider_id = physics_engine->add_cube_collider(cube_transform_id, vec3(0.2, 0.2, 0.2), vec3(-7.0, 0.2, 0.0), quat(), 1.0, 0.0, 0.8, 0.7, false);

    controlled_cube = physics_engine->colliders[collider_id];

    physics_engine->update(0.016);
}

int main() {
    GLFWwindow *window = init_opengl(1000, 1000, "hi");
    Controls controls(window);
    int window_width, window_height;

    Scene scene;
    scene.camera.eye = vec3(-12.0, 8.0, 0.0);
    scene.camera.target = vec3(0.0, 0.0, 0.0);
    scene.camera.up = vec3(0.0, 1.0, 0.0);
    scene.camera.fov = 67.0;
    scene.camera.aspect = 1.0;
    scene.camera.near = 0.1;
    scene.camera.far = 100.0;
    scene.add_meshes_from_file("resources/cube.obj", &cube_mesh_ids);
    scene.add_meshes_from_file("resources/plane.obj", &plane_mesh_ids);

    Renderer renderer;
    renderer.scene = &scene;
    renderer.shader = Shader::load_from_file("shaders/preamble.glsl", "shaders/default.frag", "shaders/default.vert");

    PhysicsEngine physics_engine;
    physics_engine.scene = &scene;

    //init_catapult_scene(&physics_engine);
    //init_wall_scene(&physics_engine);
    init_jump_scene(&physics_engine);

    float camera_azimuth, camera_inclination = 0.6 * M_PI;

    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &window_width, &window_height);
        controls.update();

        if (controls.key_down[GLFW_KEY_LEFT]) {
            camera_azimuth -= 0.02;
        }
        if (controls.key_down[GLFW_KEY_RIGHT]) {
            camera_azimuth += 0.02;
        }
        if (controls.key_down[GLFW_KEY_UP]) {
            camera_inclination -= 0.02;

            if (camera_inclination < 0.1) {
                camera_inclination = 0.1;
            }
        }
        if (controls.key_down[GLFW_KEY_DOWN]) {
            camera_inclination += 0.02;

            if (camera_inclination > M_PI - 0.1) {
                camera_inclination = M_PI - 0.1;
            }
        }

        vec3 camera_direction;
        camera_direction.x = sin(camera_inclination) * cos(camera_azimuth);
        camera_direction.y = cos(camera_inclination);
        camera_direction.z = sin(camera_inclination) * sin(camera_azimuth);

        if (controls.key_down[GLFW_KEY_P] || controls.key_clicked[GLFW_KEY_O] || controlled_cube) {
            physics_engine.update(0.016);
        }

        if (controlled_cube) {
            //controlled_cube->body.orientation = quat(vec3(0.0, -1.0, 0.0), camera_azimuth);

            if (controls.key_down[GLFW_KEY_Q]) {
                controlled_cube->body.add_force_at_point(vec3(0.0, 50.0, 0.0), controlled_cube->body.position);
            }

            if (controls.key_down[GLFW_KEY_W] || controls.key_down[GLFW_KEY_A]
                    || controls.key_down[GLFW_KEY_S] || controls.key_down[GLFW_KEY_D]) {
                controlled_cube->body.velocity.x = 0.0;
                controlled_cube->body.velocity.z = 0.0;
            }

            if (controls.key_down[GLFW_KEY_W]) {
                controlled_cube->body.velocity.x += 2.0 * cosf(camera_azimuth);
                controlled_cube->body.velocity.z += 2.0 * sinf(camera_azimuth);
            }
            if (controls.key_down[GLFW_KEY_A]) {
                controlled_cube->body.velocity.x += 2.0 * cosf(camera_azimuth - 0.5 * M_PI);
                controlled_cube->body.velocity.z += 2.0 * sinf(camera_azimuth - 0.5 * M_PI);
            }
            if (controls.key_down[GLFW_KEY_S]) {
                controlled_cube->body.velocity.x += -2.0 * cosf(camera_azimuth);
                controlled_cube->body.velocity.z += -2.0 * sinf(camera_azimuth);
            }
            if (controls.key_down[GLFW_KEY_D]) {
                controlled_cube->body.velocity.x += -2.0 * cosf(camera_azimuth - 0.5 * M_PI);
                controlled_cube->body.velocity.z += -2.0 * sinf(camera_azimuth - 0.5 * M_PI);
            }

            scene.camera.eye = controlled_cube->body.position - 5.0 * camera_direction;
            scene.camera.target = controlled_cube->body.position;
        }
        else {
            if (controls.key_down[GLFW_KEY_W]) {
                scene.camera.eye.x += 0.1 * cosf(camera_azimuth);
                scene.camera.eye.z += 0.1 * sinf(camera_azimuth);
            }
            if (controls.key_down[GLFW_KEY_A]) {
                scene.camera.eye.x += 0.1 * cosf(camera_azimuth - 0.5 * M_PI);
                scene.camera.eye.z += 0.1 * sinf(camera_azimuth - 0.5 * M_PI);
            }
            if (controls.key_down[GLFW_KEY_S]) {
                scene.camera.eye.x -= 0.1 * cosf(camera_azimuth);
                scene.camera.eye.z -= 0.1 * sinf(camera_azimuth);
            }
            if (controls.key_down[GLFW_KEY_D]) {
                scene.camera.eye.x += 0.1 * cosf(camera_azimuth + 0.5 * M_PI);
                scene.camera.eye.z += 0.1 * sinf(camera_azimuth + 0.5 * M_PI);
            }
            if (controls.key_down[GLFW_KEY_Q]) {
                scene.camera.eye.y -= 0.1; 
            }
            if (controls.key_down[GLFW_KEY_E]) {
                scene.camera.eye.y += 0.1; 
            }

            scene.camera.target = scene.camera.eye + camera_direction;
        }

        renderer.resize(window_width, window_height);
        renderer.paint();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
};
