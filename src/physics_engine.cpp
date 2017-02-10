#include "physics_engine.h"

int PhysicsEngine::add_cube_collider(int transform_id, vec3 half_lengths, vec3 position, quat orientation, float mass) {
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
    collider->body.is_static = false;
    colliders.push_back(collider);
}

int PhysicsEngine::add_plane_collider(int transform_id) {
    PlaneCollider *collider = new PlaneCollider();
    collider->id = colliders.size();
    collider->transform_id = transform_id;
    collider->normal = vec3(0.0, 1.0, 0.0);
    collider->body.is_static = true;
    colliders.push_back(collider);
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

    contacts = contact_store.get_all();
}

void PhysicsEngine::resolve_contacts(float e) {
    for (int i = 0; i < contacts.size(); i++) {
        int j = rand_num() * contacts.size();
        Contact temp = contacts[i]; 
        contacts[i] = contacts[j];
        contacts[j] = temp;
    }

    for (Contact &contact : contacts) {
        contact.apply_impulses(e);
    }

    contacts.resize(0);
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
    // Reset some things for this frame
    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.is_frozen = false;
        if (colliders[i]->body.is_static) {
            colliders[i]->level = 0;
        }
        else {
            colliders[i]->level = -1;
        }

        RigidBody *body = &colliders[i]->body;
        body->add_force_at_point(vec3(0.0, -9.8 * body->mass, 0.0), body->position);
    }

    // First resolve the collisions
    for (int i = 0; i < 5; i++) {
        integrate_positions(dt);
        generate_contacts();
        resolve_contacts(0.20);
        restore_positions();
    }

    integrate_velocities(dt);

    // Create the contact graph.
    generate_contacts();
    int level = 1;
    bool body_on_level = true;
    while (body_on_level) {
        body_on_level = false;

        for (int i = 0; i < contacts.size(); i++) {
            if ((contacts[i].collider1->level == (level - 1)) && (contacts[i].collider2->level == -1)) {
                contacts[i].collider2->level = level;
                body_on_level = true;
            }

            if ((contacts[i].collider2->level == (level - 1)) && (contacts[i].collider1->level == -1)) {
                contacts[i].collider1->level = level;
                body_on_level = true;
            }
        }

        level++;
    }
    resolve_contacts(0.0);

    // Resolve the contacts.
    for (int i = 0; i < 10; i++) {
        integrate_positions(dt);
        generate_contacts();
        resolve_contacts(-1.0 + ((i + 1.0) / 10.0));
        restore_positions();
    }

    // Do shock propagation using the collision graph
    for (int i = 0; i < level; i++) {
        for (int j = 0; j < 100; j++) {
            integrate_positions(dt);
            generate_contacts();
            resolve_contacts(0.0);
            restore_positions();
        }

        for (int j = 0; j < colliders.size(); j++) {
            if (colliders[j]->level == i) {
                colliders[j]->body.is_frozen = true;
            }
        }
    }
    for (int i = 0; i < colliders.size(); i++) {
        colliders[i]->body.is_frozen = false;
    }

    integrate_positions(dt);

    // Update the transforms for the renderer and reset the forces for the bodies.
    for (int i = 0; i < colliders.size(); i++) {
        Collider *collider = colliders[i];
        collider->update_transform(&(scene->transforms[collider->transform_id]));

        RigidBody *body = &collider->body;
        body->reset_forces();
    }
}
