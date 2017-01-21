#pragma once

#include "maths.h"

class RigidBody {
    public:
        float mass;
        mat4 inertia_tensor;
        mat4 inverse_inertia_tensor;

        vec3 position;
        quat orientation;
        vec3 velocity;
        vec3 angular_velocity;

        vec3 prev_position;
        quat prev_orientation;
        vec3 prev_velocity;
        vec3 prev_angular_velocity;

        vec3 force_accumulator;
        vec3 torque_accumulator;

        bool is_frozen;
        bool is_static;

        void apply_impulse(vec3 j, vec3 r);
        void integrate_position(float dt); 
        void integrate_velocity(float dt); 
        void reset_forces();
        void add_force_at_point(const vec3 &force, const vec3 &point);
        void restore_position();
};

