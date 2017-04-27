#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "tiny_obj_loader.h"
#include "maths.h"
#include "texture.h"

struct Material;
struct Transform;

struct Mesh {
    std::string name;

    int num_vertices;

    GLuint vao;
    GLuint position_vbo;
    GLuint normal_vbo;
    GLuint tex_coord_vbo;

    int material_id;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    GLuint diffuse_map;
};

struct Camera {
    vec3 eye;
    vec3 target;
    vec3 up;

    float fov;
    float aspect;
    float near;
    float far;

    mat4 proj_mat;
    mat4 view_mat;

    mat4 inv_proj_mat;
    mat4 inv_view_mat;
};

struct Instance {
    int mesh_id;
    int transform_id;
    bool casts_shadow, draw_outline;

    Instance();
};

struct Transform {
    vec3 scale;
    vec3 translation;
    quat orientation;

    Transform();
};

class Scene {
    public:
        Camera camera;
        std::vector<Mesh> meshes;
        std::vector<Instance> instances;
        std::vector<Transform> transforms;
        std::vector<Material> materials;

        Scene();
        void add_meshes_from_file(std::string file_name, std::vector<int> *mesh_ids);
        void update_camera_matrices();
        int add_mesh(Mesh mesh);
        int add_material(Material material);
        int add_instance(int mesh_id);
};
