#pragma once

#include "maths.h"

class RigidBody {
    public:
        float mass;
        mat4 inertia_tensor;

        vec3 position;
        quat orientation;
        vec3 velocity;
        vec3 angular_velocity;

        vec3 force_accumulator;
        vec3 torque_accumulator;

        float restitution;
        float friction;

        bool is_static;

        RigidBody();
        float get_inv_mass();
        mat4 get_inv_inertia_tensor();
        void apply_impulse(const vec3 &impulse);
        void apply_rotational_impulse(const vec3 &point, const vec3 &impulse);
        void add_force_at_point(const vec3 &force, const vec3 &point);
        void reset_forces();
        void update(float dt);

        static mat4 create_box_inertia_tensor(float mass, const vec3 &half_lengths);
        static mat4 create_sphere_inertia_tensor(float mass, float radius);
};
