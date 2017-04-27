#include <math.h>

#include "maths.h"

float rand_num() {
    return (rand() % 1000) / 1000.0;
}

/*
 * 3-D vectors
 */

vec3::vec3() {
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
}

vec3::vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

vec3::vec3(float v[3]) {
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
}

vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

vec3 operator*(float s , const vec3 &v) {
    return vec3(s * v.x, s * v.y, s * v.z);
}

float vec3::dot(const vec3 &v1, const vec3 &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 vec3::normalize() const {
    float l = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);

    if (l == 0) {
        return vec3(0, 0, 0);
    }

    return vec3(this->x / l, this->y / l, this->z / l);
}

vec3 vec3::cross(const vec3 &v1, const vec3 &v2) {
    float x = v1.y * v2.z - v1.z * v2.y;
    float y = v1.z * v2.x - v1.x * v2.z;
    float z = v1.x * v2.y - v1.y * v2.x;
    return vec3(x, y, z);
}

void vec3::print() const {
    printf("<%f, %f, %f>\n", this->x, this->y, this->z);
}

float vec3::length() const {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

float vec3::length_squared() const {
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

/*
 * 4-D vectors
 */

vec4::vec4() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
    w = 0.0;
}

vec4::vec4(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

void vec4::print() {
    printf("<%f, %f, %f, %f>\n", this->x, this->y, this->z, this->w);
}

/*
 * 4 x 4 matrices
 */

mat4::mat4() {
    this->m[0] = 1.0;
    this->m[1] = 0.0;
    this->m[2] = 0.0;
    this->m[3] = 0.0;
    this->m[4] = 0.0;
    this->m[5] = 1.0;
    this->m[6] = 0.0;
    this->m[7] = 0.0;
    this->m[8] = 0.0;
    this->m[9] = 0.0;
    this->m[10] = 1.0;
    this->m[11] = 0.0;
    this->m[12] = 0.0;
    this->m[13] = 0.0;
    this->m[14] = 0.0;
    this->m[15] = 1.0;
}

mat4::mat4(float a, float b, float c, float d,
        float e, float f, float g, float h,
        float i, float j, float k, float l,
        float m, float n, float o, float p) {
    this->m[0] = a;
    this->m[1] = b;
    this->m[2] = c;
    this->m[3] = d;
    this->m[4] = e;
    this->m[5] = f;
    this->m[6] = g;
    this->m[7] = h;
    this->m[8] = i;
    this->m[9] = j;
    this->m[10] = k;
    this->m[11] = l;
    this->m[12] = m;
    this->m[13] = n;
    this->m[14] = o;
    this->m[15] = p;
}

mat4 mat4::translation(const vec3 &v) {
    return mat4(
            1, 0, 0, v.x,
            0, 1, 0, v.y,
            0, 0, 1, v.z,
            0, 0, 0, 1
            );
}

mat4 mat4::scale(const vec3 &v) {
    return mat4(
            v.x, 0,   0,   0,
            0,   v.y, 0,   0,
            0,   0,   v.z, 0,
            0,   0,   0,   1
            );
}

/*
 * https://en.wikipedia.org/wiki/Rotation_matrix
 */
mat4 mat4::rotation_x(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
            1, 0, 0, 0,    
            0, c, -s, 0,
            0, s, c, 0,
            0, 0, 0, 1
            );
}

mat4 mat4::rotation_y(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
            c, 0, s, 0,    
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1
            );
}

mat4 mat4::rotation_z(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
            c, -s, 0, 0,    
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
            );
}

void mat4::print() {
    float *a = this->m;
    printf("[%f, %f, %f, %f]\n", a[0], a[1], a[2], a[3]);
    printf("[%f, %f, %f, %f]\n", a[4], a[5], a[6], a[7]);
    printf("[%f, %f, %f, %f]\n", a[8], a[9], a[10], a[11]);
    printf("[%f, %f, %f, %f]\n", a[12], a[13], a[14], a[15]);
}

mat4 mat4::transpose() {
    float *a = this->m;
    return mat4(
            a[0], a[4], a[8], a[12],
            a[1], a[5], a[9], a[13],
            a[2], a[6], a[10], a[14],
            a[3], a[7], a[11], a[15]
            );
}

mat4 mat4::normal_transform() {
    return this->inverse().transpose();
}

/*
 * http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
 */
mat4 mat4::inverse() {
    float *a = this->m;

    float det = 
        a[0]*a[5]*a[10]*a[15] +
        a[0]*a[6]*a[11]*a[13] +
        a[0]*a[7]*a[9]*a[14] +

        a[1]*a[4]*a[11]*a[14] +
        a[1]*a[6]*a[8]*a[15] +
        a[1]*a[7]*a[10]*a[12] +

        a[2]*a[4]*a[9]*a[15] +
        a[2]*a[5]*a[11]*a[12] +
        a[2]*a[7]*a[8]*a[13] +

        a[3]*a[4]*a[10]*a[13] +
        a[3]*a[5]*a[8]*a[14] +
        a[3]*a[6]*a[9]*a[12] -

        a[0]*a[5]*a[11]*a[14] -
        a[0]*a[6]*a[9]*a[15] -
        a[0]*a[7]*a[10]*a[13] -

        a[1]*a[4]*a[10]*a[15] -
        a[1]*a[6]*a[11]*a[12] -
        a[1]*a[7]*a[8]*a[14] -

        a[2]*a[4]*a[11]*a[13] -
        a[2]*a[5]*a[8]*a[15] -
        a[2]*a[7]*a[9]*a[12] -

        a[3]*a[4]*a[9]*a[14] -
        a[3]*a[5]*a[10]*a[12] -
        a[3]*a[6]*a[8]*a[13]; 

    float a0 = a[5]*a[10]*a[15] + a[6]*a[11]*a[13] + a[7]*a[9]*a[14]
        - a[5]*a[11]*a[14] - a[6]*a[9]*a[15] - a[7]*a[10]*a[13];

    float a1 = a[1]*a[11]*a[14] + a[2]*a[9]*a[15] + a[3]*a[10]*a[13]
        - a[1]*a[10]*a[15] - a[2]*a[11]*a[13] - a[3]*a[9]*a[14];

    float a2 = a[1]*a[6]*a[15] + a[2]*a[7]*a[13] + a[3]*a[5]*a[14]
        - a[1]*a[7]*a[14] - a[2]*a[5]*a[15] - a[3]*a[6]*a[13];

    float a3 = a[1]*a[7]*a[10] + a[2]*a[5]*a[11] + a[3]*a[6]*a[9]
        - a[1]*a[6]*a[11] - a[2]*a[7]*a[9] - a[3]*a[5]*a[10];

    float a4 = a[4]*a[11]*a[14] + a[6]*a[8]*a[15] + a[7]*a[10]*a[12]
        - a[4]*a[10]*a[15] - a[6]*a[11]*a[12] - a[7]*a[8]*a[14];

    float a5 = a[0]*a[10]*a[15] + a[2]*a[11]*a[12] + a[3]*a[8]*a[14]
        - a[0]*a[11]*a[14] - a[2]*a[8]*a[15] - a[3]*a[10]*a[12];

    float a6 = a[0]*a[7]*a[14] + a[2]*a[4]*a[15] + a[3]*a[6]*a[12]
        - a[0]*a[6]*a[15] - a[2]*a[7]*a[12] - a[3]*a[4]*a[14];

    float a7 = a[0]*a[6]*a[11] + a[2]*a[7]*a[8] + a[3]*a[4]*a[10]
        - a[0]*a[7]*a[10] - a[2]*a[4]*a[11] - a[3]*a[6]*a[8];

    float a8 = a[4]*a[9]*a[15] + a[5]*a[11]*a[12] + a[7]*a[8]*a[13]
        - a[4]*a[11]*a[13] - a[5]*a[8]*a[15] - a[7]*a[9]*a[12];

    float a9 = a[0]*a[11]*a[13] + a[1]*a[8]*a[15] + a[3]*a[9]*a[12]
        - a[0]*a[9]*a[15] - a[1]*a[11]*a[12] - a[3]*a[8]*a[13];

    float a10 = a[0]*a[5]*a[15] + a[1]*a[7]*a[12] + a[3]*a[4]*a[13]
        - a[0]*a[7]*a[13] - a[1]*a[4]*a[15] - a[3]*a[5]*a[12];

    float a11 = a[0]*a[7]*a[9] + a[1]*a[4]*a[11] + a[3]*a[5]*a[8]
        - a[0]*a[5]*a[11] - a[1]*a[7]*a[8] - a[3]*a[4]*a[9];

    float a12 = a[4]*a[10]*a[13] + a[5]*a[8]*a[14] + a[6]*a[9]*a[12]
        - a[4]*a[9]*a[14] - a[5]*a[10]*a[12] - a[6]*a[8]*a[13];

    float a13 = a[0]*a[9]*a[14] + a[1]*a[10]*a[12] + a[2]*a[8]*a[13]
        - a[0]*a[10]*a[13] - a[1]*a[8]*a[14] - a[2]*a[9]*a[12];

    float a14 = a[0]*a[6]*a[13] + a[1]*a[4]*a[14] + a[2]*a[5]*a[12]
        - a[0]*a[5]*a[14] - a[1]*a[6]*a[12] - a[2]*a[4]*a[13];

    float a15 = a[0]*a[5]*a[10] + a[1]*a[6]*a[8] + a[2]*a[4]*a[9]
        - a[0]*a[6]*a[9] - a[1]*a[4]*a[10] - a[2]*a[5]*a[8];

    return mat4( 
            a0 / det, a1 / det, a2 / det, a3 / det,
            a4 / det, a5 / det, a6 / det, a7 / det,
            a8 / det, a9 / det, a10 / det, a11 / det, 
            a12 / det, a13 / det, a14 / det, a15 / det
            );
}

/*
 * http://www.cs.virginia.edu/~gfx/Courses/1999/intro.fall99.html/lookat.html
 */
mat4 mat4::look_at(const vec3 &eye, const vec3 &target, const vec3 &up) {
    vec3 up_p = up.normalize();
    vec3 f = (target - eye).normalize();
    vec3 s = vec3::cross(f, up_p).normalize();
    vec3 u = vec3::cross(s, f).normalize();

    mat4 M = mat4(
            s.x, s.y, s.z, 0,
            u.x, u.y, u.z, 0,
            -f.x, -f.y, -f.z, 0,
            0, 0, 0, 1
            );
    mat4 T = mat4::translation(vec3(-eye.x, -eye.y, -eye.z));
    return M * T;
}

/*
 * http://www.cs.virginia.edu/~gfx/Courses/2000/intro.spring00.html/lectures/lecture09/sld017.htm
 */
mat4 mat4::perspective_projection(float fov, float aspect, float near, float far) {
    float f = 1.0 / tan(fov * DEGREES_PER_RADIAN / 2.0);
    float denominator = near - far;

    float a = f / aspect;
    float b = f;
    float c = (far + near) / denominator;
    float d = (2.0 * far * near) / denominator;

    return mat4(
            a, 0, 0, 0,
            0, b, 0, 0,
            0, 0, c, d,
            0, 0, -1, 0
            );
}

/*
 * https://en.wikipedia.org/wiki/Orthographic_projection
 */
mat4 mat4::orthographic_projection(float left, float right, float bottom, float top, float near, float far) {
    float a = 2 / (right - left);
    float b = 2 / (top - bottom);
    float c = -2 / (far - near);
    float d = -(right + left) / (right - left);
    float e = -(top + bottom) / (top - bottom);
    float f = -(far + near) / (far - near);

    return mat4(
            a, 0, 0, d,
            0, b, 0, e,
            0, 0, c, f,
            0, 0, 0, 1
            );
}

mat4 operator*(const mat4 &m1, const mat4 &m2) {
    const float *a = m1.m;
    const float *b = m2.m;

    float c0 = a[0]*b[0] + a[1]*b[4] + a[2]*b[8] + a[3]*b[12];
    float c1 = a[0]*b[1] + a[1]*b[5] + a[2]*b[9] + a[3]*b[13];
    float c2 = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
    float c3 = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];

    float c4 = a[4]*b[0] + a[5]*b[4] + a[6]*b[8] + a[7]*b[12];
    float c5 = a[4]*b[1] + a[5]*b[5] + a[6]*b[9] + a[7]*b[13];
    float c6 = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
    float c7 = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];

    float c8 = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
    float c9 = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
    float c10 = a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
    float c11 = a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];

    float c12 = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
    float c13 = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
    float c14 = a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
    float c15 = a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];

    return mat4(
            c0, c1, c2, c3,
            c4, c5, c6, c7,
            c8, c9, c10, c11,
            c12, c13, c14, c15
            );
}

vec3 operator*(const mat4 &m, const vec3 &v) {
    float x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z + m.m[3];
    float y = m.m[4] * v.x + m.m[5] * v.y + m.m[6] * v.z + m.m[7];
    float z = m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z + m.m[11];
    return vec3(x, y, z);
}

vec4 operator*(const mat4 &m, const vec4 &v) {
    float x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z + m.m[3] * v.w;
    float y = m.m[4] * v.x + m.m[5] * v.y + m.m[6] * v.z + m.m[7] * v.w;
    float z = m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z + m.m[11] * v.w;
    float w = m.m[12] * v.x + m.m[13] * v.y + m.m[14] * v.z + m.m[15] * v.w;
    return vec4(x, y, z, w);
}

float vec3::operator[](int i) const {
    if (i == 0) {
        return x;
    }
    else if (i == 1) {
        return y;
    }
    return z;
}

float& vec3::operator[](int i) {
    if (i == 0) {
        return x;
    }
    else if (i == 1) {
        return y;
    }
    return z;
}

quat::quat() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 1;
}

quat::quat(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

quat::quat(const vec3 &v, float theta) {
    float temp = sin(theta / 2.0);
    x = temp * v.x;
    y = temp * v.y;
    z = temp * v.z;
    w = cos(theta / 2.0);
    *this = this->normalize();
}

quat quat::normalize() {
    float mag = sqrt(this->w*this->w + this->x*this->x + this->y*this->y + this->z*this->z);

    if (mag == 0) {
        return *this;
    }

    return quat(this->x / mag, this->y / mag, this->z / mag, this->w / mag);
}

mat4 quat::get_matrix() {
    float x = this->x;
    float y = this->y;
    float z = this->z;
    float w = this->w;

    return mat4(1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z      , 2.0 * x * z + 2.0 * w * y      , 0.0,
                2.0 * x * y + 2.0 * w * z      , 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z - 2.0 * w * x      , 0.0, 
                2.0 * x * z - 2.0 * w * y      , 2.0 * y * z + 2.0 * w * x      , 1.0 - 2.0 * x * x - 2.0 * y * y, 0.0, 
                0.0                            , 0.0                            , 0.0                            , 1.0);
}

void quat::print() {
    printf("<%f, %f, %f, %f>\n", this->x, this->y, this->z, this->z);
}

quat operator*(const quat &u, const quat &v) {
    float x = v.w * u.x + v.x * u.w - v.y * u.z + v.z * u.y;
    float y = v.w * u.y + v.x * u.z + v.y * u.w - v.z * u.x;
    float z = v.w * u.z - v.x * u.y + v.y * u.x + v.z * u.w;
    float w = v.w * u.w - v.x * u.x - v.y * u.y - v.z * u.z;

    return quat(x, y, z, w).normalize();
}

vec3 ray::point_at_time(float t) {
    return origin + t * direction;
}
