#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "physics_engine.h"
#include "physics_scene_editor.h"
#include "controls.h"

static std::vector<int> cube_mesh_ids;
static std::vector<int> plane_mesh_ids;
static std::vector<int> sphere_mesh_ids;
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

void init_jump_scene(PhysicsEngine *physics_engine) {
    int instance_id, transform_id, collider_id;
    Collider *collider;
    Scene *scene = physics_engine->scene;

    float restitution = 0.5;
    float friction = 0.2;

    {
        instance_id = scene->add_instance(plane_mesh_ids[0]);
        scene->instances[instance_id].casts_shadow = false;
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_plane_collider(transform_id);
        collider = physics_engine->colliders[collider_id];

        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = true;
    }

    {
        instance_id = scene->add_instance(cube_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_cube_collider(transform_id, vec3(1.0, 0.7, 1.0));
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(-4.0, 0.7, 0.0);
        collider->body.orientation = quat(vec3(0.0, 1.0, 0.0), 0.75);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = true;
        collider->body.inertia_tensor = RigidBody::create_box_inertia_tensor(1.0, vec3(1.0, 0.7, 1.0));
    }

    {
        instance_id = scene->add_instance(cube_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_cube_collider(transform_id, vec3(1.0, 1.4, 1.0));
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(-1.0, 1.4, 0.0);
        collider->body.orientation = quat(vec3(0.0, 1.0, 0.0), 0.55);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = true;
        collider->body.inertia_tensor = RigidBody::create_box_inertia_tensor(1.0, vec3(1.0, 1.4, 1.0));
    }

    {
        instance_id = scene->add_instance(cube_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_cube_collider(transform_id, vec3(1.0, 2.1, 1.0));
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(2.0, 2.1, -1.0);
        collider->body.orientation = quat(vec3(0.0, 1.0, 0.0), 0.25);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = true;
        collider->body.inertia_tensor = RigidBody::create_box_inertia_tensor(1.0, vec3(1.0, 2.1, 1.0));
    }

    {
        instance_id = scene->add_instance(cube_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_cube_collider(transform_id, vec3(1.0, 2.8, 1.0));
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(5.0, 2.8, -2.0);
        collider->body.orientation = quat(vec3(0.0, 1.0, 0.0), 0.83);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = true;
        collider->body.inertia_tensor = RigidBody::create_box_inertia_tensor(1.0, vec3(1.0, 2.8, 1.0));
    }

    {
        /*
        instance_id = scene->add_instance(sphere_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_sphere_collider(transform_id, 0.2);
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(-7.0, 0.2, 0.2);
        collider->body.orientation = quat(vec3(1.0, 0.0, 0.0), 0.0);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = false;
        collider->body.inertia_tensor = RigidBody::create_sphere_inertia_tensor(1.0, 0.2);
        */

        instance_id = scene->add_instance(cube_mesh_ids[0]);
        transform_id = scene->instances[instance_id].transform_id;
        collider_id = physics_engine->add_cube_collider(transform_id, vec3(0.2, 0.2, 0.2));
        collider = physics_engine->colliders[collider_id];

        collider->body.position = vec3(-7.0, 0.2, 0.2);
        collider->body.orientation = quat(vec3(1.0, 0.0, 0.0), 0.0);
        collider->body.mass = 1.0;
        collider->body.restitution = restitution;
        collider->body.friction = friction;
        collider->body.is_static = false;
        collider->body.inertia_tensor = RigidBody::create_box_inertia_tensor(1.0, vec3(0.2, 0.2, 0.2));

        controlled_cube = physics_engine->colliders[collider_id];
    }


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
    scene.add_meshes_from_file("resources/sphere.obj", &sphere_mesh_ids);
    scene.box_mesh_id = cube_mesh_ids[0];
    scene.sphere_mesh_id = sphere_mesh_ids[0];

    Renderer renderer;
    renderer.scene = &scene;
    renderer.shader = Shader::load_from_file("shaders/preamble.glsl", "shaders/default.frag", "shaders/default.vert");

    PhysicsEngine physics_engine;
    physics_engine.scene = &scene;

    PhysicsSceneEditor physics_scene_editor(&physics_engine, &controls);

    init_jump_scene(&physics_engine);

    float camera_azimuth = 0.0, camera_inclination = 0.6 * M_PI;

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
            if (controls.key_down[GLFW_KEY_Q]) {
                controlled_cube->body.velocity.y += 0.7;
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

        scene.update_camera_matrices();

        {
            mat4 inv_proj_mat = scene.camera.inv_proj_mat;
            mat4 inv_view_mat = scene.camera.inv_view_mat;

            vec4 direction;
            direction.x = -1.0 + 2 * controls.mouse_pos_x / window_width;
            direction.y = 1.0 - (2.0 * controls.mouse_pos_y) / window_height;

            direction = inv_proj_mat * direction;
            direction.z = -1.0;
            direction.w = 0.0;

            direction = inv_view_mat * direction;

            controls.mouse_ray.direction = vec3(direction.x, direction.y, direction.z);
            controls.mouse_ray.direction.normalize();
            controls.mouse_ray.origin = scene.camera.eye;
        }

        physics_scene_editor.update(0.016);

        renderer.resize(window_width, window_height);
        renderer.paint();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
};
