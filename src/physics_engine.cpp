#include "physics_engine.h"
    
RigidBody::RigidBody() {
    transform_id = -1;
}

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
    mat4 rotation_mat = orientation.get_matrix();
    mat4 inertia_tensor_world = rotation_mat * inertia_tensor * rotation_mat.inverse();

    prev_position = position;
    prev_orientation = orientation;

    position = position + dt * velocity;
    orientation = quat(angular_velocity.normalize(), angular_velocity.length() * dt) * orientation;
}

void RigidBody::integrate_velocity(float dt) {
    mat4 rotation_mat = orientation.get_matrix();
    mat4 inverse_inertia_tensor_world = rotation_mat * inertia_tensor.inverse() * rotation_mat.inverse();

    vec3 linear_acceleration = (1.0 / mass) * force_accumulator;
    velocity = velocity + dt * linear_acceleration;

    vec3 angular_acceleration = inverse_inertia_tensor_world * torque_accumulator;
    angular_velocity = angular_velocity + dt * angular_acceleration;
}

vec3 RigidBody::get_point_in_world_space(const vec3 &point) {
    mat4 transformation = mat4::translation(position) * orientation.get_matrix();
    return transformation * point;
}

vec3 RigidBody::get_point_in_body_space(const vec3 &point) {
    mat4 transformation = mat4::translation(position) * orientation.get_matrix();
    return transformation.inverse() * point;
}

void RigidBody::restore_position() {
    position = prev_position;
    orientation = prev_orientation;
}

int PhysicsEngine::add_rigid_body(RigidBody rigid_body) {
    rigid_bodies.push_back(rigid_body);
    return rigid_bodies.size() - 1;
}

void PhysicsEngine::generate_contacts() {
    vec3 points[8] = {
        vec3( 0.5,  0.5,  0.5),
        vec3( 0.5,  0.5, -0.5),
        vec3( 0.5, -0.5,  0.5),
        vec3( 0.5, -0.5, -0.5),
        vec3(-0.5,  0.5,  0.5),
        vec3(-0.5,  0.5, -0.5),
        vec3(-0.5, -0.5,  0.5),
        vec3(-0.5, -0.5, -0.5)
    };

    for (int i = 0; i < rigid_bodies.size(); i++) {
        RigidBody *body = &rigid_bodies[i];
        mat4 transformation = mat4::translation(body->position) * body->orientation.get_matrix();

        for (int j = 0; j < 8; j++) {
            vec3 point_world = transformation * points[j];

            if (point_world.y <= 0) {
                Contact contact;
                contact.body1 = body;
                contact.body2 = NULL;
                contact.position = point_world;
                contact.normal = vec3(0.0, 1.0, 0.0);
                contacts.push_back(contact);
            }
        }
    }
}

void PhysicsEngine::resolve_contacts(float e) {
    for (int i = 0; i < contacts.size(); i++) {
        int j = rand_num() * contacts.size();
        Contact temp = contacts[i]; 
        contacts[i] = contacts[j];
        contacts[j] = temp;
    }

    for (Contact &contact : contacts) {
        RigidBody *b = contact.body1;

        float m = b->mass;
        mat4 R = b->orientation.get_matrix();
        mat4 I = R * b->inertia_tensor * R.inverse();
        vec3 r = contact.position - b->position;
        vec3 n = contact.normal;
        vec3 w = b->angular_velocity;
        vec3 v = b->velocity + vec3::cross(w, r);

        float j = MAX(-(1.0 + e) * vec3::dot(m * v, n), 0.0);
        j = MAX(-(1.0 + e) * vec3::dot(v, n) / ((1.0 / m) + vec3::dot(I.inverse() * vec3::cross(vec3::cross(r, n), r), n)), 0.0);
        b->velocity = b->velocity + (j / m) * n; 
        b->angular_velocity = b->angular_velocity + j * (I.inverse() * vec3::cross(r, n));

        vec3 t;
        if (vec3::dot(v, n) != 0.0) {
            t = v - vec3::dot(v, n) * n;
        }
        else {
            vec3 f = vec3(0.0, -9.8, 0.0);
            t = f - vec3::dot(f, n) * n;
        }
        t = t.normalize();

        float us = 0.2;
        float ud = 0.1;

        float jf = m * vec3::dot(v, t);
        if (jf >= us * j) {
            jf = -ud * j;
        }
        b->velocity = b->velocity + (jf / m) * t; 
        b->angular_velocity = b->angular_velocity + jf * (I.inverse() * vec3::cross(r, t));
    }

    contacts.resize(0);
}

void PhysicsEngine::integrate_positions(float dt) {
    for (int i = 0; i < rigid_bodies.size(); i++) {
        rigid_bodies[i].integrate_position(dt);
    }
}

void PhysicsEngine::integrate_velocities(float dt) {
    for (int i = 0; i < rigid_bodies.size(); i++) {
        rigid_bodies[i].integrate_velocity(dt);
    }
}

void PhysicsEngine::restore_positions() {
    for (int i = 0; i < rigid_bodies.size(); i++) {
        rigid_bodies[i].restore_position();
    }
}

void PhysicsEngine::run_physics(float dt) {
    for (int i = 0; i < rigid_bodies.size(); i++) {
        rigid_bodies[i].add_force_at_point(vec3(0.0, -9.8, 0.0), rigid_bodies[i].position);
    }

    for (int i = 0; i < 5; i++) {
        integrate_positions(dt);
        generate_contacts();
        resolve_contacts(0.2);
        restore_positions();
    }

    integrate_velocities(dt);

    for (int i = 0; i < 10; i++) {
        integrate_positions(dt);
        generate_contacts();
        resolve_contacts(-1.0 + ((i + 1.0) / 10.0));
        restore_positions();
    }

    integrate_positions(dt);

    for (int i = 0; i < rigid_bodies.size(); i++) {
        rigid_bodies[i].reset_forces();
    }
}
