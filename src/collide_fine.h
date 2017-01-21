#pragma once

#include <float.h>

#include "rigid_body.h"
#include "scene.h"

class Contact;
class BoxCollider;
class PlaneCollider;

class Collider {
    public:
        int transform_id;
        int level;
        RigidBody body;

        virtual void update_transform(Transform *transform) = 0;
        virtual bool collide(Collider *collider, std::vector<Contact> *contacts) = 0;
        virtual bool collideWith(BoxCollider *collider, std::vector<Contact> *contacts) = 0;   
        virtual bool collideWith(PlaneCollider *collider, std::vector<Contact> *contacts) = 0;   
};

class BoxCollider : public Collider {
    private:
        float transform_to_axis(BoxCollider *collider, vec3 axis);
        float penetration_on_axis(BoxCollider *colldier1, BoxCollider *colldier2, vec3 axis, vec3 to_center);

    public: 
        vec3 half_lengths;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, std::vector<Contact> *contacts);
        virtual bool collideWith(BoxCollider *collider, std::vector<Contact> *contacts);   
        virtual bool collideWith(PlaneCollider *collider, std::vector<Contact> *contacts);   
};

class PlaneCollider : public Collider {
    public:
        vec3 normal;

        virtual void update_transform(Transform *transform);
        virtual bool collide(Collider *collider, std::vector<Contact> *contacts);
        virtual bool collideWith(BoxCollider *collider, std::vector<Contact> *contacts);   
        virtual bool collideWith(PlaneCollider *collider, std::vector<Contact> *contacts);   
};

class Contact {
    public:
        Collider *collider1;
        Collider *collider2;

        vec3 position;
        vec3 normal;

        int flavor;
        float penetration;

        void apply_impulses(float e);
};
