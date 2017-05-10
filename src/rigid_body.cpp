#include "rigid_body.h"

RigidBody::RigidBody() {
    position = vec3(0.0, 0.0, 0.0);
    orientation = quat(vec3(1.0, 0.0, 0.0), 0.0);
    mass = 1.0;
    inertia_tensor = mat4::identity();
    restitution = 0.0;
    friction = 0.0;
    is_static = false;
}

void RigidBody::add_force_at_point(const vec3 &force, const vec3 &point) {
    force_accumulator = force_accumulator + force;
}

float RigidBody::get_inv_mass() {
    if (is_static || mass == 0.0) {
        return 0.0;
    }
    
    return 1.0 / mass;
}

mat4 RigidBody::get_inv_inertia_tensor() {
    if (is_static || mass == 0.0) {
        return mat4::zero();
    }

    return inertia_tensor.inverse();
}

void RigidBody::update(float dt) {
    if (is_static) {
        return;
    }

    vec3 acceleration = (1.0 / mass) * force_accumulator;
    velocity = velocity + dt * acceleration;
    velocity = 0.98 * velocity;
    position = position + dt * velocity;

    vec3 angular_acceleration = inertia_tensor.inverse() * torque_accumulator;
    angular_velocity = angular_velocity + dt * angular_acceleration;
    angular_velocity = 0.98 * angular_velocity;
    orientation = quat(angular_velocity, dt) * orientation;
}

void RigidBody::reset_forces() {
    force_accumulator = vec3(0.0, 0.0, 0.0);
    torque_accumulator = vec3(0.0, 0.0, 0.0);
}

void RigidBody::apply_impulse(const vec3 &impulse) {
    if (is_static) {
        return;
    }

    velocity = velocity + impulse;
}

void RigidBody::apply_rotational_impulse(const vec3 &point, const vec3 &impulse) {
    if (is_static) {
        return;
    }

    vec3 torque = vec3::cross(point - position, impulse);
    vec3 angular_acceleration = inertia_tensor.inverse() * torque;
    angular_velocity = angular_velocity + angular_acceleration;
}


mat4 RigidBody::create_box_inertia_tensor(float mass, const vec3 &half_lengths) {
    return mat4(
            (1.0 / 12.0) * mass * (half_lengths.x + half_lengths.y), 0.0, 0.0, 0.0,
            0.0, (1.0 / 12.0) * mass * (half_lengths.y + half_lengths.z), 0.0, 0.0,
            0.0, 0.0, (1.0 / 12.0) * mass * (half_lengths.z + half_lengths.x), 0.0,
            0.0, 0.0, 0.0, 1.0
            );
}

mat4 RigidBody::create_sphere_inertia_tensor(float mass, float radius) {
    return mat4(
            (2.0 / 5.0) * mass * radius * radius, 0.0, 0.0, 0.0,
            0.0, (2.0 / 5.0) * mass * radius * radius, 0.0, 0.0,
            0.0, 0.0, (2.0 / 5.0) * mass * radius * radius, 0.0,
            0.0, 0.0, 0.0, 1.0
            );
}
