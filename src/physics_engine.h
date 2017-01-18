#pragma once

#include <vector>
#include <float.h>

#include "maths.h"

class RigidBody {
    public:
        int transform_id;

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

        vec3 half_widths;

        int level;
        bool asleep;

        RigidBody();

        void integrate_position(float dt); 
        void integrate_velocity(float dt); 
        void reset_forces();
        void add_force_at_point(const vec3 &force, const vec3 &point);
        vec3 get_point_in_world_space(const vec3 &point);
        vec3 get_point_in_body_space(const vec3 &point);
        void restore_position();
};

class Contact {
    public:
        RigidBody *body1;
        RigidBody *body2;

        vec3 position;
        vec3 normal;

        int flavor;

        float penetration;
};

class PhysicsEngine {
    public:
        std::vector<RigidBody> rigid_bodies;
        std::vector<Contact> contacts;

        int add_rigid_body(RigidBody rigid_body);
        void generate_contacts();
        void resolve_contacts(float e);
        void integrate_positions(float dt);
        void integrate_velocities(float dt);
        void restore_positions();
        void run_physics(float dt);
        float penetration_on_axis(RigidBody *body1, RigidBody *body2, vec3 axis, vec3 to_center);
        float transform_to_axis(RigidBody *body, vec3 axis);
        void check_for_collision(RigidBody *body1, RigidBody *body2);
};
