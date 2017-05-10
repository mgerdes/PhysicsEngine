#pragma once

#include <vector>

#include "maths.h"
#include "scene.h"
#include "collide_fine.h"

class PhysicsEngine {
    private:
        ContactStore contact_store;

        void generate_contacts();
    public:
        std::vector<Collider*> colliders;
        Scene *scene;

        void init_contact_manifolds();
        int add_cube_collider(int transform_id, const vec3 &half_lengths);
        int add_sphere_collider(int transform_id, float radius);
        int add_plane_collider(int transform_id);

        void update(float dt);
};
