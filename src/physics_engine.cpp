#include "physics_engine.h"

int PhysicsEngine::add_cube_collider(int transform_id, vec3 half_lengths, vec3 position, quat orientation, float mass, float restitution, float us, float ud, bool is_static) {
    BoxCollider *collider = new BoxCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->half_lengths = half_lengths;
    collider->body.position = position;
    collider->body.orientation = orientation;
    collider->body.mass = mass;
    collider->body.inertia_tensor = mat4(
            (1.0 / 12.0) * mass * (half_lengths.x + half_lengths.y), 0.0, 0.0, 0.0,
            0.0, (1.0 / 12.0) * mass * (half_lengths.y + half_lengths.z), 0.0, 0.0,
            0.0, 0.0, (1.0 / 12.0) * mass * (half_lengths.z + half_lengths.x), 0.0,
            0.0, 0.0, 0.0, 1.0
            );
    collider->body.restitution = restitution;
    collider->body.friction = us;
    collider->body.is_static = is_static;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

int PhysicsEngine::add_plane_collider(int transform_id) {
    PlaneCollider *collider = new PlaneCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->normal = vec3(0.0, 1.0, 0.0);
    collider->body.is_static = true;
    collider->body.restitution = 0.5;
    collider->body.mass = 1.0;
    collider->body.friction = 0.2;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

int PhysicsEngine::add_sphere_collider(int transform_id, vec3 position, float radius, float mass, float restitution, float us, float ud, bool is_static) {
    SphereCollider *collider = new SphereCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->radius = radius;
    collider->body.position = position;
    collider->body.mass = mass;
    collider->body.friction = us;
    collider->body.inertia_tensor = mat4(
            (2.0 / 5.0) * mass * radius * radius, 0.0, 0.0, 0.0,
            0.0, (2.0 / 5.0) * mass * radius * radius, 0.0, 0.0,
            0.0, 0.0, (2.0 / 5.0) * mass * radius * radius, 0.0,
            0.0, 0.0, 0.0, 1.0
            );

    collider->body.restitution = restitution;
    collider->body.is_static = is_static;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

void PhysicsEngine::generate_contacts() {
    contact_store.empty();

    for (int i = 0; i < colliders.size(); i++) {
        Collider *collider1 = colliders[i];
        for (int j = i + 1; j < colliders.size(); j++) {
            Collider *collider2 = colliders[j];
            collider1->collide(collider2, &contact_store);
        }
    }
}

void PhysicsEngine::integrate_positions(float dt) {
    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.integrate_position(dt);
    }
}

void PhysicsEngine::integrate_velocities(float dt) {
    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.integrate_velocity(dt);
    }
}

void PhysicsEngine::restore_positions() {
    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.restore_position();
    }
}

void PhysicsEngine::update(float dt) {
    for (int i = 0; i < colliders.size(); i++) {
        RigidBody *body = &colliders[i]->body;
        body->add_force_at_point(vec3(0.0, -9.81 * body->mass, 0.0), body->position);
    }

    for (int k = 0; k < 20; k++) {
        generate_contacts();
        std::vector<Contact> contacts = contact_store.get_all();
        for (int i = 0; i < contacts.size(); i++) {
            Contact *contact = &contacts[i];

            Collider *collider1 = contact->collider1;
            Collider *collider2 = contact->collider2;

            RigidBody *b1 = &collider1->body;
            RigidBody *b2 = &collider2->body;

            if (b1->is_static && b2->is_static) {
                continue;
            }

            float inv_mass_1 = b1->get_inv_mass();
            float inv_mass_2 = b2->get_inv_mass();

            float inv_mass_sum = inv_mass_1 + inv_mass_2; 

            vec3 r1 = contact->position - b1->position;
            vec3 r2 = contact->position - b2->position;

            mat4 i1 = b1->get_inv_inertia_tensor();
            mat4 i2 = b2->get_inv_inertia_tensor();

            vec3 relative_velocity = (b2->velocity + vec3::cross(b2->angular_velocity, r2)) 
                - (b1->velocity + vec3::cross(b1->angular_velocity, r1));
            vec3 relative_normal = contact->normal;

            if (vec3::dot(relative_velocity, relative_normal) > 0.0) {
                continue;
            }

            float e = MIN(b1->restitution, b2->restitution);
            float numerator = -(1.0 + e) * vec3::dot(relative_velocity, relative_normal);
            float d1 = inv_mass_sum;
            vec3 d2 = vec3::cross(i1 * vec3::cross(r1, relative_normal), r1);
            vec3 d3 = vec3::cross(i2 * vec3::cross(r2, relative_normal), r2); 
            float denominator = d1 + vec3::dot(relative_normal, d2 + d3);

            float j = numerator / denominator;
            if (denominator == 0.0) {
                j = 0.0;
            }

            vec3 impulse = j * relative_normal;

            b1->apply_impulse((-1.0 * inv_mass_1) * impulse);
            b2->apply_impulse(inv_mass_2 * impulse);

            b1->angular_velocity = b1->angular_velocity - i1 * vec3::cross(r1, impulse);
            b2->angular_velocity = b2->angular_velocity + i2 * vec3::cross(r2, impulse);

            vec3 t = relative_velocity - vec3::dot(relative_velocity, relative_normal) * relative_normal;
            if (ABS(t.length_squared()) < 0.001) {
                continue;
            }
            t = t.normalize();

            numerator = -vec3::dot(relative_velocity, t);
            d1 = inv_mass_sum;
            d2 = vec3::cross(i1 * vec3::cross(r1, t), r1);
            d3 = vec3::cross(i2 * vec3::cross(r2, t), r2);
            denominator = d1 + vec3::dot(t, d2 + d3);
            if (denominator == 0.0) {
                continue;
            }

            float jt = numerator / denominator;
            if (ABS(jt) < 0.001) {
                continue;
            }

            float friction = sqrt(b1->friction * b2->friction);
            if (jt > j * friction) {
                jt = j * friction;
            }
            if (jt < -j * friction) {
                jt = -j * friction;
            }

            vec3 tangent_impulse = jt * t;

            b1->apply_impulse((-1.0 * inv_mass_1) * tangent_impulse);
            b2->apply_impulse(inv_mass_2 * tangent_impulse);

            b1->angular_velocity = b1->angular_velocity - i1 * vec3::cross(r1, tangent_impulse);
            b2->angular_velocity = b2->angular_velocity + i2 * vec3::cross(r2, tangent_impulse);
        }

    }

    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.update(dt);
    }

    {
        generate_contacts();
        std::vector<Contact> contacts = contact_store.get_all();
        for (int i = 0; i < contacts.size(); i++) {
            Contact *contact = &contacts[i];

            RigidBody *body1 = &contact->collider1->body; 
            RigidBody *body2 = &contact->collider2->body; 

            float inv_mass_1 = body1->get_inv_mass();
            float inv_mass_2 = body2->get_inv_mass();

            float depth = MAX(contact->penetration - 0.01, 0.0);
            float scalar = depth / (inv_mass_1 + inv_mass_2);
            vec3 correction = 0.05 * scalar * contact->normal;

            if (!body1->is_static) {
                body1->position = body1->position - inv_mass_1 * correction;
            }

            if (!body2->is_static) {
                body2->position = body2->position + inv_mass_2 * correction;
            }
        }
    }

    for (int i = 0; i < colliders.size(); i++) {
        Collider *collider = colliders[i];
        collider->update_transform(&(scene->transforms[collider->transform_id]));

        RigidBody *body = &collider->body;
        body->apply_dampening();
        body->reset_forces();
    }
}
