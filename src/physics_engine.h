#pragma once

#include <vector>

#include "maths.h"
#include "scene.h"
#include "collide_fine.h"

class PhysicsEngine {
    private:
        std::vector<Collider*> colliders;
        std::vector<Contact> contacts;

        void generate_contacts();
        void resolve_contacts(float e);
        void integrate_positions(float dt);
        void integrate_velocities(float dt);
        void restore_positions();
    public:
        Scene *scene;

        int add_cube_collider(int transform_id, vec3 half_lengths, vec3 position, quat orientation, float mass);
        int add_plane_collider(int transform_id);

        void update(float dt);
};
