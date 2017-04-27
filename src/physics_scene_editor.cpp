#include "physics_scene_editor.h"

PhysicsSceneEditor::PhysicsSceneEditor(PhysicsEngine *physics_engine, Controls *controls) {
    this->physics_engine = physics_engine;
    this->controls = controls;
    selected_collider_id = -1;
    hovered_collider_id = -1;
    selected_axis = -1;
}

void PhysicsSceneEditor::update(float dt) {
    static vec3 axes[3] = {
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    };

    Scene *scene = physics_engine->scene;

    if (controls->right_mouse_clicked) {
        if (selected_collider_id == -1) {
            // Find which collider is hovered in this frame
            float min_t = FLT_MAX;
            for (int i = 0; i < physics_engine->colliders.size(); i++) {
                float t;
                if (physics_engine->colliders[i]->intersect(controls->mouse_ray, &t)) {
                    if (t < min_t) {
                        selected_collider_id = i;
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
        }
    }

    if (controls->left_mouse_down && selected_axis == -1) {
        if (selected_collider_id != -1) {
            float t;
            Collider *selected_collider = physics_engine->colliders[selected_collider_id];

            for (int axis = 0; axis < 3; axis++) {
                vec3 ball_position = selected_collider->body.position + axes[axis];
                //printf("%d: ", axis);
                //axes[axis].print();
                if (controls->mouse_ray.intersect_sphere(ball_position, 0.1, &t)) {
                    selected_axis = axis;
                }
            }
        }
    }


    if (selected_axis != -1) {
        Collider *selected_collider = physics_engine->colliders[selected_collider_id];
        Transform *selected_transform = &scene->transforms[selected_collider->transform_id];

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

    if (!controls->left_mouse_down && selected_axis != -1) {
        selected_axis = -1;
    }
}
