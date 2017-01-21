#include "rigid_body.h"

void RigidBody::add_force_at_point(const vec3 &force, const vec3 &point) {
    force_accumulator = force_accumulator + force;

    vec3 torque = vec3::cross(point - position, force);
    torque_accumulator = torque_accumulator + torque;
}

void RigidBody::reset_forces() {
    force_accumulator = vec3(0.0, 0.0, 0.0);
    torque_accumulator = vec3(0.0, 0.0, 0.0);
}

void RigidBody::integrate_position(float dt) {
    if (is_static || is_frozen) {
        return;
    }

    mat4 rotation_mat = orientation.get_matrix();
    mat4 inertia_tensor_world = rotation_mat * inertia_tensor * rotation_mat.inverse();

    prev_position = position;
    prev_orientation = orientation;

    position = position + dt * velocity;
    orientation = quat(angular_velocity.normalize(), angular_velocity.length() * dt) * orientation;
}

void RigidBody::integrate_velocity(float dt) {
    if (is_static || is_frozen) {
        return;
    }

    mat4 rotation_mat = orientation.get_matrix();
    mat4 inverse_inertia_tensor_world = rotation_mat * inertia_tensor.inverse() * rotation_mat.inverse();

    vec3 linear_acceleration = (1.0 / mass) * force_accumulator;
    velocity = velocity + dt * linear_acceleration;

    vec3 angular_acceleration = inverse_inertia_tensor_world * torque_accumulator;
    angular_velocity = angular_velocity + dt * angular_acceleration;
}

void RigidBody::restore_position() {
    position = prev_position;
    orientation = prev_orientation;
}

void RigidBody::apply_impulse(vec3 j, vec3 r) {
    if (is_static || is_frozen) {
        return;
    }

    mat4 R = orientation.get_matrix();
    float m_inv = 1.0 / mass;
    mat4 I_inv = (R * inertia_tensor * R.inverse()).inverse();

    velocity = velocity + m_inv * j; 
    angular_velocity = angular_velocity + (I_inv * vec3::cross(r, j));
}
