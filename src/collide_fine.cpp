#include "collide_fine.h"

void ContactStore::add(Contact contact) {
    contacts.push_back(contact);
}

void ContactStore::empty() {
    contacts.resize(0);
}

std::vector<Contact> ContactStore::get_all() {
    return contacts;
}

void BoxCollider::update_transform(Transform *transform) {
    transform->scale = 2.0 * half_lengths;
    transform->translation = body.position;
    transform->orientation = body.orientation;
}

bool BoxCollider::collide(Collider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

float BoxCollider::transform_to_axis(BoxCollider *collider, vec3 axis) {
    mat4 transformation = collider->body.orientation.get_matrix();

    vec3 axes[3];
    axes[0] = vec3(transformation.m[0], transformation.m[4], transformation.m[8]);
    axes[1] = vec3(transformation.m[1], transformation.m[5], transformation.m[9]);
    axes[2] = vec3(transformation.m[2], transformation.m[6], transformation.m[10]);

    return collider->half_lengths.x * ABS(vec3::dot(axes[0], axis))
        + collider->half_lengths.y * ABS(vec3::dot(axes[1], axis))
        + collider->half_lengths.z * ABS(vec3::dot(axes[2], axis));
}

float BoxCollider::penetration_on_axis(BoxCollider *collider1, BoxCollider *collider2, vec3 axis, vec3 to_center) {
    float body1_projection = transform_to_axis(collider1, axis);
    float body2_projection = transform_to_axis(collider2, axis);
    float distance = ABS(vec3::dot(to_center, axis));
    return body1_projection + body2_projection - distance;
}

bool BoxCollider::collide_with(SphereCollider *collider, ContactStore *contact_store) {
    mat4 transformation = mat4::translation(body.position) * body.orientation.get_matrix();

    vec3 center = collider->body.position;
    vec3 relative_center = transformation.inverse() * center;

    if (ABS(relative_center.x) - collider->radius > half_lengths.x ||
            ABS(relative_center.y) - collider->radius > half_lengths.y ||
            ABS(relative_center.z) - collider->radius > half_lengths.z) {
        return false;
    }

    vec3 closest_pt = vec3(0.0, 0.0, 0.0);
    float dist;

    dist = relative_center.x;
    if (dist > half_lengths.x) dist = half_lengths.x;
    if (dist < -half_lengths.x) dist = -half_lengths.x;
    closest_pt.x = dist;

    dist = relative_center.y;
    if (dist > half_lengths.y) dist = half_lengths.y;
    if (dist < -half_lengths.y) dist = -half_lengths.y;
    closest_pt.y = dist;

    dist = relative_center.z;
    if (dist > half_lengths.z) dist = half_lengths.z;
    if (dist < -half_lengths.z) dist = -half_lengths.z;
    closest_pt.z = dist;

    dist = (closest_pt - relative_center).length_squared();
    if (dist > collider->radius * collider->radius) {
        return false;
    }

    vec3 closest_pt_world = transformation * closest_pt;

    Contact contact;
    contact.collider1 = collider;
    contact.collider2 = this;
    contact.normal = (closest_pt_world - center).normalize();
    contact.position = closest_pt_world;

    contact_store->add(contact);

    return true;
}

bool BoxCollider::collide_with(BoxCollider *collider, ContactStore *contact_store) {
    RigidBody body1 = this->body;
    RigidBody body2 = collider->body;

    mat4 transformation1 = body1.orientation.get_matrix();
    mat4 transformation2 = body2.orientation.get_matrix();

    vec3 axes[15];

    axes[0] = vec3(transformation1.m[0], transformation1.m[4], transformation1.m[8]);
    axes[1] = vec3(transformation1.m[1], transformation1.m[5], transformation1.m[9]);
    axes[2] = vec3(transformation1.m[2], transformation1.m[6], transformation1.m[10]);

    axes[3] = vec3(transformation2.m[0], transformation2.m[4], transformation2.m[8]);
    axes[4] = vec3(transformation2.m[1], transformation2.m[5], transformation2.m[9]);
    axes[5] = vec3(transformation2.m[2], transformation2.m[6], transformation2.m[10]);

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
    int best_single_axis_case;
    vec3 to_center = body2.position - body1.position;

    for (int k = 0; k < 15; k++) {
        vec3 axis = axes[k];

        if (axis.length() < 0.001) {
            continue;
        }
        axis = axis.normalize();

        float overlap = penetration_on_axis(this, collider, axis, to_center);
        if (overlap < 0) {
            return false;
        }
        if (overlap < best_overlap) {
            best_overlap = overlap;
            best_case = k;
        }

        if (k == 5) {
            best_single_axis_case = best_case;
        }
    }

    if (best_case < 3) {
        vec3 normal = axes[best_case];
        if (vec3::dot(to_center, normal) > 0) {
            normal = -1.0 * normal; 
        }

        vec3 vertex = collider->half_lengths;

        if (vec3::dot(axes[3], normal) < 0) {
            vertex.x = -vertex.x;
        }
        if (vec3::dot(axes[4], normal) < 0) {
            vertex.y = -vertex.y;
        }
        if (vec3::dot(axes[5], normal) < 0) {
            vertex.z = -vertex.z;
        }

        vertex = mat4::translation(body2.position) * transformation2 * vertex;

        Contact contact;
        contact.collider1 = collider;
        contact.collider2 = this;
        contact.position = vertex;
        contact.normal = normal;
        contact.penetration = best_overlap;
        contact.collision_case = best_case;
        contact_store->add(contact);
        return true;
    }
    else if (best_case < 6) {
        to_center = -1.0 * to_center;

        vec3 normal = axes[best_case];
        if (vec3::dot(to_center, normal) > 0) {
            normal = -1.0 * normal; 
        }

        vec3 vertex = this->half_lengths;

        if (vec3::dot(axes[0], normal) < 0) {
            vertex.x = -vertex.x;
        }
        if (vec3::dot(axes[1], normal) < 0) {
            vertex.y = -vertex.y;
        }
        if (vec3::dot(axes[2], normal) < 0) {
            vertex.z = -vertex.z;
        }

        vertex = mat4::translation(body1.position) * transformation1 * vertex;

        Contact contact;
        contact.collider1 = this;
        contact.collider2 = collider;
        contact.position = vertex;
        contact.normal = normal;
        contact.penetration = best_overlap;
        contact_store->add(contact);
        return true;
    }
    else {
        int one_axis_index = (best_case - 6) / 3;
        int two_axis_index = (best_case - 6) % 3;

        vec3 one_axis = axes[one_axis_index];
        vec3 two_axis = axes[two_axis_index + 3];
        vec3 axis = vec3::cross(one_axis, two_axis).normalize();

        if (vec3::dot(axis, to_center) > 0) {
            axis = -1.0 * axis;
        }

        vec3 pt_on_edge1 = this->half_lengths;
        vec3 pt_on_edge2 = collider->half_lengths;
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

        pt_on_edge1 = mat4::translation(body1.position) * transformation1 * pt_on_edge1;
        pt_on_edge2 = mat4::translation(body2.position) * transformation2 * pt_on_edge2;

        float sm_one = one_axis.length_squared();
        float sm_two = two_axis.length_squared();
        float dot_product_edges = vec3::dot(one_axis, two_axis);

        vec3 to_st = pt_on_edge1 - pt_on_edge2;
        float dp_sta_one = vec3::dot(axes[one_axis_index], to_st);
        float dp_sta_two = vec3::dot(axes[two_axis_index + 3], to_st);

        float denom = sm_one * sm_two - dot_product_edges * dot_product_edges;

        if (ABS(denom) < 0.0001f) {
            Contact contact;
            contact.collider1 = this;
            contact.collider2 = collider;
            contact.position = best_single_axis_case > 2 ? pt_on_edge1 : pt_on_edge2; 
            contact.normal = -1.0 * axis;
            contact.penetration = best_overlap;
            contact_store->add(contact);
            return true;
        }

        float a = (dot_product_edges * dp_sta_two - sm_two * dp_sta_one) / denom;
        float b = (sm_one * dp_sta_two - dot_product_edges * dp_sta_one) / denom;

        float body_1_half_width = this->half_lengths[one_axis_index];
        float body_2_half_width = collider->half_lengths[two_axis_index];

        if (a > body_1_half_width || a < -body_1_half_width || b > body_2_half_width || b < -body_2_half_width) {
            Contact contact;
            contact.collider1 = this;
            contact.collider2 = collider;
            contact.position = best_single_axis_case > 2 ? pt_on_edge1 : pt_on_edge2; 
            contact.normal = -1.0 * axis;
            contact.penetration = best_overlap;
            contact_store->add(contact);
            return true;
        }
        else {
            vec3 nearest_pt_on_one = pt_on_edge1 + a * one_axis;
            vec3 nearest_pt_on_two = pt_on_edge2 + b * two_axis;

            Contact contact;
            contact.collider1 = this;
            contact.collider2 = collider;
            contact.position = 0.5 * nearest_pt_on_one + 0.5 * nearest_pt_on_two;
            contact.normal = -1.0 * axis;
            contact.penetration = best_overlap;
            contact_store->add(contact);
            return true;
        }
    }

    return false;
}

bool BoxCollider::collide_with(PlaneCollider *collider, ContactStore *contact_store) {
    mat4 transformation = mat4::translation(body.position) * body.orientation.get_matrix();

    vec3 points[8] = {
        vec3( half_lengths.x,   half_lengths.y,   half_lengths.z),
        vec3( half_lengths.x,   half_lengths.y,  -half_lengths.z),
        vec3( half_lengths.x,  -half_lengths.y,   half_lengths.z),
        vec3( half_lengths.x,  -half_lengths.y,  -half_lengths.z),
        vec3(-half_lengths.x,   half_lengths.y,   half_lengths.z),
        vec3(-half_lengths.x,   half_lengths.y,  -half_lengths.z),
        vec3(-half_lengths.x,  -half_lengths.y,   half_lengths.z),
        vec3(-half_lengths.x,  -half_lengths.y,  -half_lengths.z),
    };

    bool found_collision = false;

    for (int i = 0; i < 8; i++) {
        vec3 point_world = transformation * points[i];

        if (point_world.y <= 0) {
            found_collision = true;

            Contact contact;
            contact.collider1 = this;
            contact.collider2 = collider;
            contact.position = point_world;
            contact.normal = vec3(0.0, -1.0, 0.0);
            contact.penetration = 0.0;

            contact_store->add(contact);
        }
    }

    return found_collision;
}

bool BoxCollider::intersect(ray r, float *t_out) {
    mat4 inv_orientation_matrix = body.orientation.get_matrix().inverse();

    vec3 ro = inv_orientation_matrix * (r.origin - body.position);
    vec3 rd = inv_orientation_matrix * r.direction;

    vec3 min = -1.0 * half_lengths;
    vec3 max = half_lengths;

    float t0x = (min.x - ro.x) / rd.x;
    float t1x = (max.x - ro.x) / rd.x;

    if (t0x > t1x) {
        float temp = t0x;
        t0x = t1x;
        t1x = temp;
    }

    float t0y = (min.y - ro.y) / rd.y;
    float t1y = (max.y - ro.y) / rd.y;

    if (t0y > t1y) {
        float temp = t0y;
        t0y = t1y;
        t1y = temp;
    }

    float t0z = (min.z - ro.z) / rd.z;
    float t1z = (max.z - ro.z) / rd.z;

    if (t0z > t1z) {
        float temp = t0z;
        t0z = t1z;
        t1z = temp;
    }

    float t0 = t0x;
    float t1 = t1x;

    if (t0y > t0) {
        t0 = t0y;
    }
    if (t1y < t1) {
        t1 = t1y;
    }

    if (t0z > t0) {
        t0 = t0z;
    }
    if (t1z < t1) {
        t1 = t1z;
    }

    if (t0 < 0.0 || t0 > t1) {
        return false;
    }
    
    *t_out = t0;
    return true;
}

void PlaneCollider::update_transform(Transform *transform) {
    transform->scale = vec3(100.0, 1.0, 100.0);
    transform->translation = vec3(0.0, -0.01, 0.0);
}

bool PlaneCollider::collide(Collider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool PlaneCollider::collide_with(SphereCollider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool PlaneCollider::collide_with(BoxCollider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool PlaneCollider::collide_with(PlaneCollider *collider, ContactStore *contact_store) {
    return false;
}

bool PlaneCollider::intersect(ray r, float *t_out) {
    return false;
}

void SphereCollider::update_transform(Transform *transform) {
    transform->scale = vec3(radius, radius, radius);
    transform->translation = body.position;
    transform->orientation = body.orientation;
}

bool SphereCollider::collide(Collider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool SphereCollider::collide_with(SphereCollider *collider, ContactStore *contact_store) {
    vec3 v1 = this->body.position;
    vec3 v2 = collider->body.position;
    vec3 r = v2 - v1;

    if (r.length() > collider->radius + this->radius) {
        return false;
    }

    Contact contact;
    contact.collider1 = this;
    contact.collider2 = collider;
    contact.normal = r.normalize();
    contact.position = v1 + this->radius * contact.normal;

    contact_store->add(contact);

    return true;
}

bool SphereCollider::collide_with(BoxCollider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool SphereCollider::collide_with(PlaneCollider *collider, ContactStore *contact_store) {
    if (this->body.position.y - this->radius < -0.02) {
        Contact contact;
        contact.collider1 = this;
        contact.collider2 = collider;
        contact.position = vec3(this->body.position.x, this->body.position.y - this->radius + 0.02, this->body.position.z);
        contact.normal = vec3(0.0, -1.0, 0.0);
        contact.penetration = 0.0;
        contact_store->add(contact);
        return true;
    }
    return false;
}

bool SphereCollider::intersect(ray r, float *t_out) {
    float a = vec3::dot(r.direction, r.direction);
    float b = 2.0 * (vec3::dot(r.direction, r.origin - body.position));
    float c = vec3::dot(r.origin - body.position, r.origin - body.position) - radius * radius;
    float det = b * b - 4 * a * c;

    if (det < 0.0) {
        return false;
    }

    float t1 = (-b + sqrt(det)) / (2.0 * a);
    float t2 = (-b - sqrt(det)) / (2.0 * a);
    float t = t1 < t2 ? t1 : t2;

    if (t < 0.0) {
        return false;
    }

    *t_out = t;
    return true;
}

void Contact::apply_impulses() {
    this->apply_impulses(MAX(collider1->restitution, collider2->restitution));
}

void Contact::apply_impulses(float e) {
    RigidBody *b1 = &collider1->body;
    RigidBody *b2 = &collider2->body;

    mat4 R1 = b1->orientation.get_matrix();
    mat4 R2 = b2->orientation.get_matrix();

    float m1_inv;
    mat4 I1_inv;
    if (b1->is_static || b1->is_frozen) {
        m1_inv = 0.0;
        I1_inv = mat4(
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0
                );
    }
    else {
        m1_inv = 1.0 / b1->mass;
        I1_inv = (R1 * b1->inertia_tensor * R1.inverse()).inverse();
    }

    float m2_inv;
    mat4 I2_inv;
    if (b2->is_static || b2->is_frozen) {
        m2_inv = 0.0;
        I2_inv = mat4(
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0
                );
    }
    else {
        m2_inv = 1.0 / b2->mass;
        I2_inv = (R2 * b2->inertia_tensor * R2.inverse()).inverse();
    }

    vec3 r1 = position - b1->position;
    vec3 r2 = position - b2->position;

    vec3 w1 = b1->angular_velocity;
    vec3 v1 = b1->velocity + vec3::cross(w1, r1);

    vec3 w2 = b2->angular_velocity;
    vec3 v2 = b2->velocity + vec3::cross(w2, r2);

    vec3 vr = v2 - v1;
    vec3 n = normal;

    float temp1 = m1_inv + vec3::dot(I1_inv * vec3::cross(vec3::cross(r1, n), r1), n);
    float temp2 = m2_inv + vec3::dot(I2_inv * vec3::cross(vec3::cross(r2, n), r2), n);

    float j = MAX(-(1.0 + e) * vec3::dot(vr, n) / (temp1 + temp2), 0.0);

    b1->apply_impulse(-1.0 * j * n, r1);
    b2->apply_impulse(j * n, r2);

    vec3 t1;
    if (vec3::dot(vr, n) != 0.0) {
        t1 = vr - vec3::dot(vr, n) * n;
    }
    else {
        vec3 f = b1->force_accumulator;
        t1 = f - vec3::dot(f, n) * n;
    }
    t1 = t1.normalize();

    vec3 t2;
    if (vec3::dot(vr, n) != 0.0) {
        t2 = vr - vec3::dot(vr, n) * n;
    }
    else {
        vec3 f = b2->force_accumulator;
        t2 = f - vec3::dot(f, n) * n;
    }
    t2 = t2.normalize();

    float us = MAX(collider1->us, collider2->us);
    float ud = MAX(collider1->ud, collider2->ud);

    float jf1 = b1->mass * vec3::dot(vr, t1);
    if (jf1 >= us * j) {
        jf1 = -ud * j;
    }
    b1->apply_impulse(-1.0 * jf1 * t1, r1);

    float jf2 = b2->mass * vec3::dot(1.0 * vr, t2);
    if (jf2 >= us * j) {
        jf2 = -ud * j;
    }
    b2->apply_impulse(jf2 * t2, r2);
}
