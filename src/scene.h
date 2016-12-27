#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

#include "tiny_obj_loader.h"
#include "maths.h"
#include "texture.h"

struct Mesh {
    std::string name;

    GLuint vao;
    GLuint position_vbo;
    GLuint normal_vbo;
    GLuint tex_coord_vbo;

    int num_vertices;
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
};

struct Instance {
    int mesh_id;
    int transform_id;
};

struct Transform {
    vec3 scale;
    vec3 translation;
    quat rotation;
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
        int add_mesh(Mesh mesh);
        int add_material(Material material);
        int add_instance(int mesh_id);
};
