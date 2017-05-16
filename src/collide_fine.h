#pragma once

#include <float.h>
#include <math.h>
#include <vector>

#include "rigid_body.h"
#include "scene.h"

class ContactManifold;
class BoxCollider;
class PlaneCollider;
class SphereCollider;

class Collider {
    public:
        int id;
        int transform_id;
        int level;
        RigidBody body;

        virtual void update_transform(Transform *transform) = 0;
        virtual ContactManifold collide(Collider *collider) = 0;
        virtual ContactManifold collide_with(SphereCollider *collider) = 0;   
        virtual ContactManifold collide_with(BoxCollider *collider) = 0;   
        virtual ContactManifold collide_with(PlaneCollider *collider) = 0;   
        virtual bool intersect(ray r, float *t_out) = 0;
};

class SphereCollider : public Collider {
    public: 
        float radius;

        virtual void update_transform(Transform *transform);
        virtual ContactManifold collide(Collider *collider);
        virtual ContactManifold collide_with(SphereCollider *collider);   
        virtual ContactManifold collide_with(BoxCollider *collider);   
        virtual ContactManifold collide_with(PlaneCollider *collider);   
        virtual bool intersect(ray r, float *t_out);
};

class BoxCollider : public Collider {
    private:
        bool contains_point(const vec3 &p);
        void get_points(vec3 *points);
        void get_edges(line_segment *edges); 
        void clip_edges(line_segment *edges, std::vector<vec3> *contact_points);
        void get_planes(plane *planes);

    public: 
        vec3 half_lengths;

        virtual void update_transform(Transform *transform);
        virtual ContactManifold collide(Collider *collider);
        virtual ContactManifold collide_with(SphereCollider *collider);   
        virtual ContactManifold collide_with(BoxCollider *collider);   
        virtual ContactManifold collide_with(PlaneCollider *collider);   
        virtual bool intersect(ray r, float *t_out);
};

class PlaneCollider : public Collider {
    public:
        vec3 normal;

        virtual void update_transform(Transform *transform);
        virtual ContactManifold collide(Collider *collider);
        virtual ContactManifold collide_with(SphereCollider *collider);   
        virtual ContactManifold collide_with(BoxCollider *collider);   
        virtual ContactManifold collide_with(PlaneCollider *collider);   
        virtual bool intersect(ray r, float *t_out);
};

class Contact {
    public:
        vec3 position;
        vec3 normal;
        float penetration;
        bool is_resting_contact;
};

class ContactManifold {
    public:
        Collider *collider1;
        Collider *collider2;
        std::vector<Contact> contacts;
};
