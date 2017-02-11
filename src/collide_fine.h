#pragma once

#include <float.h>

#include "rigid_body.h"
#include "scene.h"

class Contact;
class BoxCollider;
class PlaneCollider;

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
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store) = 0;   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store) = 0;   
};

class BoxCollider : public Collider {
    private:
        float transform_to_axis(BoxCollider *collider, vec3 axis);
        float penetration_on_axis(BoxCollider *colldier1, BoxCollider *colldier2, vec3 axis, vec3 to_center);

    public: 
        vec3 half_lengths;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, ContactStore *contact_store);
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store);   
};

class PlaneCollider : public Collider {
    public:
        vec3 normal;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, ContactStore *contact_store);
        virtual bool collide_with(BoxCollider *collider, ContactStore *contact_store);   
        virtual bool collide_with(PlaneCollider *collider, ContactStore *contact_store);   
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
