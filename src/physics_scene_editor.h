#pragma once

#include "physics_engine.h"
#include "scene.h"
#include "controls.h"
#include "maths.h"

class PhysicsSceneEditor {
    private:
        PhysicsEngine *physics_engine;
        Controls *controls;
        int selected_collider_id;
        float selected_collider_distance;
        int selected_axis;
        bool is_rotating_collider, is_scaling_collider;

    public:
        PhysicsSceneEditor(PhysicsEngine *physics_engine, Controls *controls);
        void update(float dt);
};
