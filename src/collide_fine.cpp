#include "collide_fine.h"
#include "renderer.h"

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

bool BoxCollider::contains_point(const vec3 &point) {
    mat4 transformation = body.orientation.get_matrix().inverse();
    vec3 box_point = transformation * (point - body.position);
    vec3 new_half_lengths = half_lengths + vec3(0.01, 0.01, 0.01);

    if (box_point.x < -new_half_lengths.x || box_point.x > new_half_lengths.x) {
        return false;
    }

    if (box_point.y < -new_half_lengths.y || box_point.y > new_half_lengths.y) {
        return false;
    }

    if (box_point.z < -new_half_lengths.z || box_point.z > new_half_lengths.z) {
        return false;
    }

    return true;

    vec3 axes[3];
    axes[0] = vec3(transformation.m[0], transformation.m[4], transformation.m[8]).normalize();
    axes[1] = vec3(transformation.m[1], transformation.m[5], transformation.m[9]).normalize();
    axes[2] = vec3(transformation.m[2], transformation.m[6], transformation.m[10]).normalize();

    float axes_0_dist = vec3::dot(point - body.position, axes[0]);
    if (axes_0_dist < -half_lengths.x || axes_0_dist > half_lengths.x) {
        return false;
    }

    float axes_1_dist = vec3::dot(point - body.position, axes[1]);
    if (axes_1_dist < -half_lengths.y || axes_1_dist > half_lengths.y) {
        return false;
    }

    float axes_2_dist = vec3::dot(point - body.position, axes[2]);
    if (axes_2_dist < -half_lengths.z || axes_2_dist > half_lengths.z) {
        return false;
    }

    return true;
}

void BoxCollider::get_points(vec3 *points) {
    mat4 transformation = body.orientation.get_matrix();
    vec3 position = body.position;

    points[0] = transformation * vec3(half_lengths.x, half_lengths.y, half_lengths.z) + position;
    points[1] = transformation * vec3(half_lengths.x, half_lengths.y, -half_lengths.z) + position;
    points[2] = transformation * vec3(half_lengths.x, -half_lengths.y, half_lengths.z) + position;
    points[3] = transformation * vec3(half_lengths.x, -half_lengths.y, -half_lengths.z) + position;
    points[4] = transformation * vec3(-half_lengths.x, half_lengths.y, half_lengths.z) + position;
    points[5] = transformation * vec3(-half_lengths.x, half_lengths.y, -half_lengths.z) + position;
    points[6] = transformation * vec3(-half_lengths.x, -half_lengths.y, half_lengths.z) + position;
    points[7] = transformation * vec3(-half_lengths.x, -half_lengths.y, -half_lengths.z) + position;
}

void BoxCollider::get_edges(line_segment *edges) {
    static int edge_indices[12][2] = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3},
        {4, 5}, {4, 6}, {5, 7}, {6, 7},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
    };

    vec3 points[8];
    get_points(points);

    for (int i = 0; i < 12; i++) {
        edges[i] = line_segment(points[edge_indices[i][0]], points[edge_indices[i][1]]);
    }
}

void BoxCollider::get_planes(plane *planes) {
    mat4 transformation = body.orientation.get_matrix();    

    vec3 axes[3];
    axes[0] = vec3(transformation.m[0], transformation.m[4], transformation.m[8]).normalize();
    axes[1] = vec3(transformation.m[1], transformation.m[5], transformation.m[9]).normalize();
    axes[2] = vec3(transformation.m[2], transformation.m[6], transformation.m[10]).normalize();

    planes[0] = plane(body.position + half_lengths.x * axes[0], axes[0]);
    planes[1] = plane(body.position - half_lengths.x * axes[0], -1.0 * axes[0]);
    planes[2] = plane(body.position + half_lengths.y * axes[1], axes[1]);
    planes[3] = plane(body.position - half_lengths.y * axes[1], -1.0 * axes[1]);
    planes[4] = plane(body.position + half_lengths.z * axes[2], axes[2]);
    planes[5] = plane(body.position - half_lengths.z * axes[2], -1.0 * axes[2]);
}

void BoxCollider::clip_edges(line_segment *edges, std::vector<vec3> *contact_points) {
    plane planes[6];
    get_planes(planes);

    for (int i = 0; i < 6; i++) {
        plane p = planes[i];

        for (int j = 0; j < 12; j++) {
            line_segment e = edges[j];
            vec3 intersection;

            if (e.intersect_plane(p, &intersection)) {
                if (contains_point(intersection)) {
                    contact_points->push_back(intersection);
                }
            }
        }
    }
}

bool BoxCollider::collide_with(SphereCollider *collider, ContactStore *contact_store) {
    mat4 transformation = body.orientation.get_matrix();

    vec3 closest_pt_on_box = transformation.inverse() * (collider->body.position - body.position);

    if (closest_pt_on_box.x > half_lengths.x) closest_pt_on_box.x = half_lengths.x;
    if (closest_pt_on_box.x < -half_lengths.x) closest_pt_on_box.x = -half_lengths.x;

    if (closest_pt_on_box.y > half_lengths.y) closest_pt_on_box.y = half_lengths.y;
    if (closest_pt_on_box.y < -half_lengths.y) closest_pt_on_box.y = -half_lengths.y;

    if (closest_pt_on_box.z > half_lengths.z) closest_pt_on_box.z = half_lengths.z;
    if (closest_pt_on_box.z < -half_lengths.z) closest_pt_on_box.z = -half_lengths.z;

    closest_pt_on_box = (transformation * closest_pt_on_box) + body.position;

    float dist = (closest_pt_on_box - collider->body.position).length_squared();

    if (dist > collider->radius * collider->radius) {
        return false;
    }

    vec3 normal = (closest_pt_on_box - collider->body.position).normalize();
    vec3 closest_pt_on_sphere = collider->body.position + collider->radius * normal; 

    Contact contact;
    contact.collider1 = collider;
    contact.collider2 = this;
    contact.normal = normal;
    contact.position = 0.5 * (closest_pt_on_sphere + closest_pt_on_box);
    contact.penetration = (closest_pt_on_box - closest_pt_on_sphere).length();
    contact_store->add(contact);

    return true;
}

bool BoxCollider::collide_with(BoxCollider *collider, ContactStore *contact_store) {
    RigidBody body1 = this->body;
    RigidBody body2 = collider->body;

    mat4 transformation1 = body1.orientation.get_matrix();
    mat4 transformation2 = body2.orientation.get_matrix();

    vec3 axes[15];

    axes[0] = vec3(transformation1.m[0], transformation1.m[4], transformation1.m[8]).normalize();
    axes[1] = vec3(transformation1.m[1], transformation1.m[5], transformation1.m[9]).normalize();
    axes[2] = vec3(transformation1.m[2], transformation1.m[6], transformation1.m[10]).normalize();

    axes[3] = vec3(transformation2.m[0], transformation2.m[4], transformation2.m[8]).normalize();
    axes[4] = vec3(transformation2.m[1], transformation2.m[5], transformation2.m[9]).normalize();
    axes[5] = vec3(transformation2.m[2], transformation2.m[6], transformation2.m[10]).normalize();

    axes[6] = vec3::cross(axes[0], axes[3]).normalize();
    axes[7] = vec3::cross(axes[0], axes[4]).normalize();
    axes[8] = vec3::cross(axes[0], axes[5]).normalize();

    axes[9] = vec3::cross(axes[1], axes[3]).normalize();
    axes[10] = vec3::cross(axes[1], axes[4]).normalize();
    axes[11] = vec3::cross(axes[1], axes[5]).normalize();

    axes[12] = vec3::cross(axes[2], axes[3]).normalize();
    axes[13] = vec3::cross(axes[2], axes[4]).normalize();
    axes[14] = vec3::cross(axes[2], axes[5]).normalize();

    float smallest_penetration = FLT_MAX;
    int smallest_penetration_case = -1;

    for (int i = 0; i < 15; i++) {
        vec3 axis = axes[i];

        if (axis.length() < 0.0001) {
            continue;
        }

        vec3 box1_points[8];
        this->get_points(box1_points);
        float i1_min = vec3::dot(box1_points[0], axis);
        float i1_max = vec3::dot(box1_points[0], axis);
        for (int j = 1; j < 8; j++) {
            i1_min = MIN(vec3::dot(box1_points[j], axis), i1_min);
            i1_max = MAX(vec3::dot(box1_points[j], axis), i1_max);
        }

        vec3 box2_points[8];
        collider->get_points(box2_points);
        float i2_min = vec3::dot(box2_points[0], axis);
        float i2_max = vec3::dot(box2_points[0], axis);
        for (int j = 1; j < 8; j++) {
            i2_min = MIN(vec3::dot(box2_points[j], axis), i2_min);
            i2_max = MAX(vec3::dot(box2_points[j], axis), i2_max);
        }

        if (i1_min > i2_max || i2_min > i1_max) {
            return false;
        }

        float i1_length = i1_max - i1_min;
        float i2_length = i2_max - i2_min;
        float total_length = MAX(i1_max, i2_max) - MIN(i1_min, i2_min);

        float penetration = i1_length + i2_length - total_length;

        if (penetration < smallest_penetration) {
            smallest_penetration = penetration;
            smallest_penetration_case = i;
        }
    }

    std::vector<vec3> contact_points;
    line_segment box_edges[12];

    this->get_edges(box_edges); 
    collider->clip_edges(box_edges, &contact_points);

    collider->get_edges(box_edges);
    this->clip_edges(box_edges, &contact_points);

    std::vector<vec3> unique_contact_points;
    for (int i = 0; i < contact_points.size(); i++) {
        for (int j = i + 1; j < contact_points.size(); j++) {
            if ((contact_points[i] - contact_points[j]).length_squared() < 0.001) {
                break;
            }

            if (j == contact_points.size() - 1) {
                unique_contact_points.push_back(contact_points[i]);
            }
        }
    }

    for (int i = 0; i < unique_contact_points.size(); i++) {
        Contact contact; 
        contact.collider1 = this;
        contact.collider2 = collider;
        contact.position = unique_contact_points[i];
        contact.normal = axes[smallest_penetration_case];
        if (vec3::dot(contact.normal, body1.position - body2.position) > 0) {
            contact.normal = -1.0 * contact.normal;
        }
        contact.penetration = smallest_penetration;
        contact_store->add(contact);
    }

    return unique_contact_points.size() > 0;
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
            contact.penetration = -1.0 * point_world.y;

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
    contact.penetration = (collider->radius + this->radius) - r.length();
    contact.position = v1 + this->radius * contact.normal;

    contact_store->add(contact);

    return true;
}

bool SphereCollider::collide_with(BoxCollider *collider, ContactStore *contact_store) {
    return collider->collide_with(this, contact_store);
}

bool SphereCollider::collide_with(PlaneCollider *collider, ContactStore *contact_store) {
    if (body.position.y - radius < 0.0) {
        Contact contact;
        contact.collider1 = this;
        contact.collider2 = collider;
        contact.position = vec3(body.position.x, body.position.y - radius, body.position.z);
        contact.normal = vec3(0.0, -1.0, 0.0);
        contact.penetration = -(body.position.y - radius);
        contact_store->add(contact);
        return true;
    }
    return false;
}

bool SphereCollider::intersect(ray r, float *t_out) {
    return r.intersect_sphere(body.position, radius, t_out);
}
