#include "physics_scene_editor.h"

PhysicsSceneEditor::PhysicsSceneEditor(PhysicsEngine *physics_engine, Controls *controls) {
    this->physics_engine = physics_engine;
    this->controls = controls;
    selected_collider_id = -1;
    hovered_collider_id = -1;
}

void PhysicsSceneEditor::update(float dt) {
    Scene *scene = physics_engine->scene;

    // Move around the selected instance
    if (selected_collider_id != -1) {
        Collider *selected_collider = physics_engine->colliders[selected_collider_id];
        Transform *selected_transform = &scene->transforms[selected_collider->transform_id];
        selected_collider->body.position = controls->mouse_ray.point_at_time(selected_collider_distance);
        selected_collider->update_transform(selected_transform);

        if (controls->mouse_scroll_y == -1.0) {
            selected_collider_distance -= 0.8;
        }
        if (controls->mouse_scroll_y == 1.0) {
            selected_collider_distance += 0.8;
        }

        if (controls->left_mouse_clicked) {
            scene->instances[selected_collider_id].draw_outline = false;
            selected_collider_id = -1;
            return;
        }
    }

    // Remove outline from previous frames hovered instance
    if (hovered_collider_id != -1) {
        scene->instances[hovered_collider_id].draw_outline = false;
    }
    hovered_collider_id = -1;

    // Find which collider is hovered in this frame
    float min_t = FLT_MAX;
    for (int i = 0; i < physics_engine->colliders.size(); i++) {
        float t;
        if (physics_engine->colliders[i]->intersect(controls->mouse_ray, &t)) {
            if (t < min_t) {
                hovered_collider_id = i;
                min_t = t;
            }
        }
    }

    // Outline the hovered collider in this frame
    if (hovered_collider_id != -1) {
        if (selected_collider_id != -1) {
            scene->instances[selected_collider_id].draw_outline = true;
        }
        else {
            scene->instances[hovered_collider_id].draw_outline = true;
        }

        if (controls->left_mouse_clicked) {
            selected_collider_id = hovered_collider_id;
            int selected_transform_id = scene->instances[selected_collider_id].transform_id;
            selected_collider_distance = (scene->transforms[selected_transform_id].translation - scene->camera.eye).length();
        }
    }
}
