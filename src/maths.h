#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEGREES_PER_RADIAN 0.01745329251

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))

float rand_num(void);

struct vec2 {
    float x, y;
};

struct vec3 {
    float x, y, z;

    vec3();
    vec3(float x, float y, float z);
    vec3(float v[3]);

    vec3 normalize() const;
    float length() const;
    float length_squared() const;
    void print() const;

    static float dot(const vec3 &v1, const vec3 &v2);
    static vec3 cross(const vec3 &v1, const vec3 &v2);

    float operator[](int i) const;
    float& operator[](int i);
};

vec3 operator+(const vec3 &u, const vec3 &v);
vec3 operator-(const vec3 &u, const vec3 &v);
vec3 operator*(float s , const vec3 &v);

struct vec4 {
    float x, y, z, w;

    vec4();
    vec4(float x, float y, float z, float w);

    void print();
};

struct mat4 {
    float m[16];

    mat4();
    mat4(float a, float b, float c, float d,
            float e, float f, float g, float h,
            float i, float j, float k, float l,
            float m, float n, float o, float p);

    mat4 inverse();
    mat4 transpose();
    mat4 normal_transform();
    void print();

    static mat4 translation(const vec3 &v);
    static mat4 scale(const vec3 &v);
    static mat4 rotation_x(float theta);
    static mat4 rotation_y(float theta);
    static mat4 rotation_z(float theta);
    static mat4 look_at(const vec3 &eye, const vec3 &target, const vec3 &up);
    static mat4 perspective_projection(float fov, float aspect, float near, float far);
    static mat4 orthographic_projection(float right, float left, float top, float bottom, float near, float far);
};

mat4 operator*(const mat4 &m1, const mat4 &m2);
vec3 operator*(const mat4 &m, const vec3 &v);
vec4 operator*(const mat4 &m, const vec4 &v);

struct quat {
    float x, y, z, w;

    quat();
    quat(float x, float y, float z, float w);
    quat(const vec3 &v, float theta);

    quat normalize();
    mat4 get_matrix();
    void print();
};

quat operator*(const quat &u, const quat &v);

struct ray {
    vec3 origin, direction; 

    vec3 point_at_time(float t);
    bool intersect_sphere(vec3 sphere_center, float sphere_radius, float *t_out);
};

struct sphere {
    vec3 center;
    float radius;
};
