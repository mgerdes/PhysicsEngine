#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEGREES_PER_RADIAN 0.01745329251

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

float rand_num(void);

struct vec2 {
    float x, y;
};

struct vec3 {
    float x, y, z;

    vec3();
    vec3(float x, float y, float z);
    vec3(float v[3]);

    static float dot(vec3 *v1, vec3 *v2);
    static vec3 normalize(vec3 *v);
    static vec3 cross(vec3 *v1, vec3 *v2);
    static void print(vec3 *v);
};

vec3 operator+(const vec3 &u, const vec3 &v);

struct vec4 {
    float x, y, z, w;

    static void print(vec4 *v);
};

struct mat4 {
    float m[16];

    mat4();
    mat4(float a, float b, float c, float d,
            float e, float f, float g, float h,
            float i, float j, float k, float l,
            float m, float n, float o, float p);

    static mat4 translation(vec3 *v);
    static mat4 scale(vec3 *v);
    static mat4 rotation_x(float theta);
    static mat4 rotation_y(float theta);
    static mat4 rotation_z(float theta);
    static mat4 inverse(mat4 *m);
    static mat4 transpose(mat4 *m);
    static mat4 normal_transform(mat4 *m);
    static mat4 look_at(vec3 *eye, vec3 *target, vec3 *up);
    static mat4 perspective_projection(float fov, float aspect, float near, float far);
    static mat4 orthographic_projection(float right, float left, float top, float bottom, float near, float far);
    static void print(mat4 *m);
};

mat4 operator*(const mat4 &u, const mat4 &v);

struct quat {
    float x, y, z, w;

    quat();
};

struct ray {
    vec3 origin, direction; 
};

struct sphere {
    vec3 center;
    float radius;
};
