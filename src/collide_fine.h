#pragma once

#include <float.h>
#include <math.h>
#include <vector>

#include "rigid_body.h"
#include "scene.h"

class Contact;
class BoxCollider;
class PlaneCollider;
class SphereCollider;

class ContactStore {
    private:
        std::vector<Contact> contacts;

    public:
        void add(Contact contact);
        void empty();
        std::vector<Contact> get_all();
};

class Collider {
    public:
        int id;
        int transform_id;
        int level;
        float restitution;
        float us, ud;
        RigidBody body;

        virtual void update_transform(Transform *transform) = 0;
        virtual bool collide(Collider *collider, ContactStore *contact_store) = 0;
        virtual bool collide_with(SphereCollider *collider, ContactStore *contact_store) = 0;   
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store) = 0;   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store) = 0;   
        virtual bool intersect(ray r, float *t_out) = 0;
};

class SphereCollider : public Collider {
    public: 
        float radius;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, ContactStore *contact_store);
        virtual bool collide_with(SphereCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store);   
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
        virtual bool collide(Collider *collider, ContactStore *contact_store);
        virtual bool collide_with(SphereCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store);   
        virtual bool intersect(ray r, float *t_out);
};

class PlaneCollider : public Collider {
    public:
        vec3 normal;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, ContactStore *contact_store);
        virtual bool collide_with(SphereCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store);   
        virtual bool intersect(ray r, float *t_out);
};

class Contact {
    public:
        Collider *collider1;
        Collider *collider2;

        vec3 position;
        vec3 normal;

        int flavor;
        float penetration;

        int collision_case;

        void apply_impulses();
        void apply_impulses(float e);
};

class ContactManifold {
    public:
        int num_contacts;
        Contact contacts[4];
};
