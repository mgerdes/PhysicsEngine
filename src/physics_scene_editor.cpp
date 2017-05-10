#include "physics_scene_editor.h"

PhysicsSceneEditor::PhysicsSceneEditor(PhysicsEngine *physics_engine, Controls *controls) {
    this->physics_engine = physics_engine;
    this->controls = controls;
    selected_collider_id = -1;
    is_rotating_collider = false;
}

void PhysicsSceneEditor::update(float dt) {
    static vec3 axes[3] = {
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    };

    Scene *scene = physics_engine->scene;

    if (controls->key_clicked[GLFW_KEY_R]) {
        is_rotating_collider = !is_rotating_collider;
    }

    if (controls->key_clicked[GLFW_KEY_C]) {
        int instance_id = scene->add_instance(scene->box_mesh_id);
        int transform_id = scene->instances[instance_id].transform_id;
        vec3 new_collider_position = controls->mouse_ray.point_at_time(5.0);
        int collider_id = physics_engine->add_cube_collider(transform_id, vec3(1.0, 1.0, 1.0), new_collider_position, quat(), 10.0, 0.2, 0.1, 0.2, true);
        Collider *new_collider = physics_engine->colliders[collider_id];
        Transform *new_transform = &scene->transforms[transform_id];
        new_collider->update_transform(new_transform);
    }

    if (controls->right_mouse_clicked) {
        if (selected_collider_id == -1) {
            float min_t = FLT_MAX;
            for (int i = 0; i < physics_engine->colliders.size(); i++) {
                float t;
                if (physics_engine->colliders[i]->intersect(controls->mouse_ray, &t)) {
                    if (t < min_t) {
                        selected_collider_id = i;
                        selected_axis = -1;
                        min_t = t;
                    }
                }
            }

            if (selected_collider_id != -1) {
                scene->instances[selected_collider_id].draw_outline = true;
            }
        }
        else {
            scene->instances[selected_collider_id].draw_outline = false;
            selected_collider_id = -1;
            selected_axis = -1;
        }
    }

    if (selected_collider_id != -1) {
        if (selected_axis != -1) {
            Collider *selected_collider = physics_engine->colliders[selected_collider_id];
            Transform *selected_transform = &scene->transforms[selected_collider->transform_id];

            if (is_rotating_collider) {
                quat rotation = quat(axes[selected_axis], 0.01 * controls->mouse_delta_x);
                selected_collider->body.orientation = rotation * selected_collider->body.orientation;
                selected_collider->update_transform(selected_transform);
            }
            else {
                vec3 A = selected_collider->body.position;
                vec3 a = axes[selected_axis];

                vec3 B = controls->mouse_ray.origin;
                vec3 b = controls->mouse_ray.direction;

                vec3 c = B - A;

                float t = -vec3::dot(a, b) * vec3::dot(b, c) + vec3::dot(a, c) * vec3::dot(b, b);
                t /= (vec3::dot(a, a) * vec3::dot(b, b) - vec3::dot(a, b) * vec3::dot(a, b));

                selected_collider->body.position = A + t * a - axes[selected_axis];
                selected_collider->update_transform(selected_transform);
            }
        }

        if (controls->left_mouse_clicked && selected_axis != -1) {
            selected_axis = -1;
        } 
        else if (controls->left_mouse_clicked && selected_axis == -1) {
            Collider *selected_collider = physics_engine->colliders[selected_collider_id];

            for (int axis = 0; axis < 3; axis++) {
                float t;
                vec3 ball_position = selected_collider->body.position + axes[axis];
                if (controls->mouse_ray.intersect_sphere(ball_position, 0.1, &t)) {
                    selected_axis = axis;
                }
            }
        }
    }
}
