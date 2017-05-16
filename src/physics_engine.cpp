#include "physics_engine.h"

int PhysicsEngine::add_cube_collider(int transform_id, const vec3 &half_lengths) {
    BoxCollider *collider = new BoxCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->half_lengths = half_lengths;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

int PhysicsEngine::add_plane_collider(int transform_id) {
    PlaneCollider *collider = new PlaneCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

int PhysicsEngine::add_sphere_collider(int transform_id, float radius) {
    SphereCollider *collider = new SphereCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->radius = radius;
    colliders.push_back(collider);
    return colliders.size() - 1;
}

std::vector<ContactManifold> PhysicsEngine::generate_contacts() {
    std::vector<ContactManifold> manifolds;

    for (int i = 0; i < colliders.size(); i++) {
        Collider *collider1 = colliders[i];
        for (int j = i + 1; j < colliders.size(); j++) {
            Collider *collider2 = colliders[j];
            ContactManifold manifold = collider1->collide(collider2);
            if (manifold.contacts.size() > 0) {
                manifolds.push_back(manifold);
            }
        }
    }

    return manifolds;
}

void PhysicsEngine::update(float dt) {
    for (int i = 0; i < colliders.size(); i++) {
        RigidBody *body = &colliders[i]->body;
        body->add_force_at_point(vec3(0.0, -9.8 * body->mass, 0.0), body->position);
    }

    for (int k = 0; k < 10; k++) {
        std::vector<ContactManifold> manifolds = generate_contacts();

        for (int i = 0; i < manifolds.size(); i++) {
            ContactManifold *manifold = &manifolds[i];

            Collider *collider1 = manifold->collider1;
            Collider *collider2 = manifold->collider2;

            RigidBody *b1 = &collider1->body;
            RigidBody *b2 = &collider2->body;

            if (b1->is_static && b2->is_static) {
                continue;
            }

            float inv_mass_1 = b1->get_inv_mass();
            float inv_mass_2 = b2->get_inv_mass();

            float inv_mass_sum = inv_mass_1 + inv_mass_2; 

            for (int j = 0; j < manifold->contacts.size(); j++) {
                Contact *contact = &manifold->contacts[j];

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

                float j_imp = numerator / (denominator * manifold->contacts.size());
                if (denominator == 0.0) {
                    j_imp = 0.0;
                }

                vec3 impulse = j_imp * relative_normal;

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
                if (jt > j_imp * friction) {
                    jt = j_imp * friction;
                }
                if (jt < -j_imp * friction) {
                    jt = -j_imp * friction;
                }

                vec3 tangent_impulse = jt * t;

                b1->apply_impulse((-1.0 * inv_mass_1) * tangent_impulse);
                b2->apply_impulse(inv_mass_2 * tangent_impulse);

                b1->angular_velocity = b1->angular_velocity - i1 * vec3::cross(r1, tangent_impulse);
                b2->angular_velocity = b2->angular_velocity + i2 * vec3::cross(r2, tangent_impulse);
            }
        }
    }

    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.update(dt);
    }

    {
        std::vector<ContactManifold> manifolds = generate_contacts();

        for (int i = 0; i < manifolds.size(); i++) {
            ContactManifold *manifold = &manifolds[i];

            RigidBody *body1 = &manifold->collider1->body; 
            RigidBody *body2 = &manifold->collider2->body; 

            float inv_mass_1 = body1->get_inv_mass();
            float inv_mass_2 = body2->get_inv_mass();

            for (int j = 0; j < manifold->contacts.size(); j++) {
                Contact *contact = &manifold->contacts[j];

                float depth = MAX(contact->penetration - 0.005, 0.0);
                float scalar = depth / (inv_mass_1 + inv_mass_2);
                vec3 correction = 0.2 * scalar * contact->normal;

                if (!body1->is_static) {
                    body1->position = body1->position - inv_mass_1 * correction;
                }

                if (!body2->is_static) {
                    body2->position = body2->position + inv_mass_2 * correction;
                }
            }
        }
    }

    for (int i = 0; i < colliders.size(); i++) {
        Collider *collider = colliders[i];
        collider->update_transform(&(scene->transforms[collider->transform_id]));

        RigidBody *body = &collider->body;
        body->reset_forces();
    }
}
