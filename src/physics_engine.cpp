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

float PhysicsEngine::transform_to_axis(RigidBody *body, vec3 axis) {
    mat4 transformation = body->orientation.get_matrix();
    vec3 axes[3];
    axes[0] =  vec3(transformation.m[0], transformation.m[4], transformation.m[8]);
    axes[1] =  vec3(transformation.m[1], transformation.m[5], transformation.m[9]);
    axes[2] =  vec3(transformation.m[2], transformation.m[6], transformation.m[10]);

    return body->half_widths.x * ABS(vec3::dot(axes[0], axis))
        + body->half_widths.y * ABS(vec3::dot(axes[1], axis))
        + body->half_widths.z * ABS(vec3::dot(axes[2], axis));
}

float PhysicsEngine::penetration_on_axis(RigidBody *body1, RigidBody *body2, vec3 axis, vec3 to_center) {
    float body1_projection = transform_to_axis(body1, axis);
    float body2_projection = transform_to_axis(body2, axis);
    float distance = ABS(vec3::dot(to_center, axis));
    return body1_projection + body2_projection - distance;
}

void PhysicsEngine::check_for_collision(RigidBody *body1, RigidBody *body2) {
    mat4 transformation1 = body1->orientation.get_matrix();
    mat4 transformation2 = body2->orientation.get_matrix();
    vec3 axes[15];

    axes[0] =  vec3(transformation1.m[0], transformation1.m[4], transformation1.m[8]);
    axes[1] =  vec3(transformation1.m[1], transformation1.m[5], transformation1.m[9]);
    axes[2] =  vec3(transformation1.m[2], transformation1.m[6], transformation1.m[10]);

    axes[3] =  vec3(transformation2.m[0], transformation2.m[4], transformation2.m[8]);
    axes[4] =  vec3(transformation2.m[1], transformation2.m[5], transformation2.m[9]);
    axes[5] =  vec3(transformation2.m[2], transformation2.m[6], transformation2.m[10]);

    axes[6] = vec3::cross(axes[0], axes[3]);
    axes[7] = vec3::cross(axes[0], axes[4]);
    axes[8] = vec3::cross(axes[0], axes[5]);

    axes[9] = vec3::cross(axes[1], axes[3]);
    axes[10] = vec3::cross(axes[1], axes[4]);
    axes[11] = vec3::cross(axes[1], axes[5]);

    axes[12] = vec3::cross(axes[2], axes[3]);
    axes[13] = vec3::cross(axes[2], axes[4]);
    axes[14] = vec3::cross(axes[2], axes[5]);

    float best_overlap = FLT_MAX;
    int best_case;
    vec3 to_center = body2->position - body1->position;

    for (int k = 0; k < 15; k++) {
        vec3 axis = axes[k];

        if (axis.length() < 0.001) {
            continue;
        }
        axis = axis.normalize();

        float overlap = penetration_on_axis(body1, body2, axis, to_center);
        if (overlap < 0) {
            return;
        }
        if (overlap < best_overlap) {
            best_overlap = overlap;
            best_case = k;
        }
    }

    if (best_case < 3) {
        vec3 normal = axes[best_case];
        if (vec3::dot(to_center, normal) > 0) {
            normal = -1.0 * normal; 
        }

        vec3 vertex = body2->half_widths;

        if (vec3::dot(axes[3], normal) < 0) {
            vertex.x = -vertex.x;
        }
        if (vec3::dot(axes[4], normal) < 0) {
            vertex.y = -vertex.y;
        }
        if (vec3::dot(axes[5], normal) < 0) {
            vertex.z = -vertex.z;
        }

        vertex = mat4::translation(body2->position) * transformation2 * vertex;

        Contact contact;
        contact.body1 = body2;
        contact.body2 = body1;
        contact.position = vertex;
        contact.normal = normal;
        contact.flavor = 0;
        contacts.push_back(contact);
    }
    else if (best_case < 6) {
        to_center = -1.0 * to_center;

        vec3 normal = axes[best_case];
        if (vec3::dot(to_center, normal) > 0) {
            normal = -1.0 * normal; 
        }

        vec3 vertex = body1->half_widths;

        if (vec3::dot(axes[0], normal) < 0) {
            vertex.x = -vertex.x;
        }
        if (vec3::dot(axes[1], normal) < 0) {
            vertex.y = -vertex.y;
        }
        if (vec3::dot(axes[2], normal) < 0) {
            vertex.z = -vertex.z;
        }

        vertex = mat4::translation(body1->position) * transformation1 * vertex;

        Contact contact;
        contact.body1 = body1;
        contact.body2 = body2;
        contact.position = vertex;
        contact.normal = normal;
        contact.flavor = 1;
        contacts.push_back(contact);
    }
    else {
        vec3 pt_on_edge1 = body1->half_widths;
        vec3 pt_on_edge2 = body2->half_widths;

        int one_axis_index = (best_case - 6) / 3;
        int two_axis_index = (best_case - 6) % 3 + 3;
        vec3 axis = axes[best_case].normalize();

        if (vec3::dot(axis, to_center) > 0) {
            axis = -1.0 * axis;
        }

        for (int i = 0; i < 3; i++) {
            if (i == one_axis_index) {
                pt_on_edge1[i] = 0.0;
            }
            else if (vec3::dot(axes[i], axis) > 0.0) {
                pt_on_edge1[i] = -pt_on_edge1[i];
            }

            if (i == two_axis_index) {
                pt_on_edge2[i] = 0.0;
            }
            else if (vec3::dot(axes[i + 3], axis) < 0.0) {
                pt_on_edge2[i] = -pt_on_edge2[i];
            }
        }

        pt_on_edge1 = mat4::translation(body1->position) * pt_on_edge1;
        pt_on_edge2 = mat4::translation(body2->position) * pt_on_edge2;

        vec3 to_st = pt_on_edge1 - pt_on_edge2;
        float dp_sta_one = vec3::dot(axes[one_axis_index], to_st);
        float dp_sta_two = vec3::dot(axes[two_axis_index], to_st);

        float sm_one = axes[one_axis_index].length_squared();
        float sm_two = axes[two_axis_index].length_squared();
        float dot_product_edges = vec3::dot(axes[one_axis_index], axes[two_axis_index]);
        float denom = sm_one * sm_two - dot_product_edges * dot_product_edges;

        if (ABS(denom) < 0.0001f) {
            Contact contact;
            contact.body1 = body1;
            contact.body2 = body2;
            contact.position = pt_on_edge1; 
            contact.normal = -1.0 * axis;
            contacts.push_back(contact);

            return;
        }

        float a = (dot_product_edges * dp_sta_two - sm_two * dp_sta_one) / denom;
        float b = (sm_one * dp_sta_two - dot_product_edges * dp_sta_one) / denom;

        vec3 nearest_pt_on_one = pt_on_edge1 + a * axes[one_axis_index];
        vec3 nearest_pt_on_two = pt_on_edge2 + b * axes[two_axis_index];

        Contact contact;
        contact.body1 = body1;
        contact.body2 = body2;
        contact.position = 0.5 * nearest_pt_on_one + 0.5 * nearest_pt_on_two;
        contact.normal = -1.0 * axis;
        contact.flavor = 2;
        contacts.push_back(contact);
    }
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
        RigidBody *body1 = &rigid_bodies[i];
        mat4 transformation1 = mat4::translation(body1->position) * body1->orientation.get_matrix();

        for (int j = 0; j < 8; j++) {
            vec3 point_world = transformation1 * points[j];

            if (point_world.y <= 0) {
                Contact contact;
                contact.body1 = body1;
                contact.body2 = NULL;
                contact.position = point_world;
                contact.normal = vec3(0.0, -1.0, 0.0);
                contacts.push_back(contact);
            }
        }

        for (int j = i + 1; j < rigid_bodies.size(); j++) {
            RigidBody *body2 = &rigid_bodies[j];
            check_for_collision(body1, body2);
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
        RigidBody *b1 = contact.body1;
        RigidBody *b2 = contact.body2;

        if (!b2) {
            float m1 = b1->mass;
            mat4 R1 = b1->orientation.get_matrix();
            mat4 I1 = R1 * b1->inertia_tensor * R1.inverse();

            vec3 r1 = contact.position - b1->position;

            vec3 w1 = b1->angular_velocity;
            vec3 v1 = b1->velocity + vec3::cross(w1, r1);

            vec3 vr = -1.0 * v1;

            vec3 n = contact.normal;

            float temp1 = (1.0 / m1) + vec3::dot(I1.inverse() * vec3::cross(vec3::cross(r1, n), r1), n);
            float j = MAX(-(1.0 + e) * vec3::dot(vr, n) / (temp1), 0.0);

            b1->velocity = b1->velocity - (j / m1) * n; 
            b1->angular_velocity = b1->angular_velocity - j * (I1.inverse() * vec3::cross(r1, n));

            vec3 t;
            if (vec3::dot(vr, n) != 0.0) {
                t = vr - vec3::dot(vr, n) * n;
            }
            else {
                vec3 f = vec3(0.0, -9.8, 0.0);
                t = f - vec3::dot(f, n) * n;
            }
            t = t.normalize();

            float us = 0.4;
            float ud = 0.3;

            float jf1 = m1 * vec3::dot(vr, t);
            if (jf1 >= us * j) {
                jf1 = -ud * j;
            }
            b1->velocity = b1->velocity - (jf1 / m1) * t; 
            b1->angular_velocity = b1->angular_velocity - jf1 * (I1.inverse() * vec3::cross(r1, t));
        }
        else {
            float m1 = b1->mass;
            mat4 R1 = b1->orientation.get_matrix();
            mat4 I1 = R1 * b1->inertia_tensor * R1.inverse();

            float m2 = b2->mass;
            mat4 R2 = b2->orientation.get_matrix();
            mat4 I2 = R2 * b2->inertia_tensor * R2.inverse();

            vec3 r1 = contact.position - b1->position;
            vec3 r2 = contact.position - b2->position;

            vec3 w1 = b1->angular_velocity;
            vec3 v1 = b1->velocity + vec3::cross(w1, r1);

            vec3 w2 = b2->angular_velocity;
            vec3 v2 = b2->velocity + vec3::cross(w2, r2);

            vec3 vr = v2 - v1;

            vec3 n = contact.normal;

            float temp1 = (1.0 / m1) + vec3::dot(I1.inverse() * vec3::cross(vec3::cross(r1, n), r1), n);
            float temp2 = (1.0 / m2) + vec3::dot(I2.inverse() * vec3::cross(vec3::cross(r2, n), r2), n);
            float j = MAX(-(1.0 + e) * vec3::dot(vr, n) / (temp1 + temp2), 0.0);

            b1->velocity = b1->velocity - (j / m1) * n; 
            b1->angular_velocity = b1->angular_velocity - j * (I1.inverse() * vec3::cross(r1, n));

            b2->velocity = b2->velocity + (j / m2) * n; 
            b2->angular_velocity = b2->angular_velocity + j * (I2.inverse() * vec3::cross(r2, n));

            if (b1 == &rigid_bodies[0]) {
                //printf("b1\n");
                //printf("n: ");
                //n.print();
                //printf("w: ");
                //b1->angular_velocity.print();
            }
            if (b2 == &rigid_bodies[0]) {
                //printf("b2\n");
                //printf("n: ");
                //n.print();
                //printf("w: ");
                //b2->angular_velocity.print();
            }

            vec3 t;
            if (vec3::dot(vr, n) != 0.0) {
                t = vr - vec3::dot(vr, n) * n;
            }
            else {
                vec3 f = vec3(0.0, -9.8, 0.0);
                t = f - vec3::dot(f, n) * n;
            }
            t = t.normalize();

            float us = 0.4;
            float ud = 0.3;

            float jf1 = m1 * vec3::dot(vr, t);
            if (jf1 >= us * j) {
                jf1 = -ud * j;
            }
            b1->velocity = b1->velocity - (jf1 / m1) * t; 
            b1->angular_velocity = b1->angular_velocity - jf1 * (I1.inverse() * vec3::cross(r1, t));

            float jf2 = m2 * vec3::dot(vr, t);
            if (jf2 >= us * j) {
                jf2 = -ud * j;
            }
            b2->velocity = b2->velocity + (jf2 / m2) * t; 
            b2->angular_velocity = b2->angular_velocity + jf2 * (I2.inverse() * vec3::cross(r2, t));
        }
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
        rigid_bodies[i].add_force_at_point(vec3(0.0, -9.8 * rigid_bodies[i].mass, 0.0), rigid_bodies[i].position);
    }

    for (int i = 0; i < 5; i++) {
        integrate_positions(dt);
        generate_contacts();
        resolve_contacts(0.3);
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
