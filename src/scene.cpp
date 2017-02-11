#include "scene.h"

Instance::Instance() {
    casts_shadow = true;
    draw_outline = true;
}

Transform::Transform() {
    scale = vec3(1.0, 1.0, 1.0);
}

Scene::Scene() {
}

void Scene::add_meshes_from_file(std::string file_name, std::vector<int> *mesh_ids) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file_name.c_str());

    for (int i = 0; i < shapes.size(); i++) {
        int material_id;
        std::vector<float> positions, normals, tex_coords;

        for (int j = 0; j < shapes[i].mesh.num_face_vertices.size(); j++) {
            for (int k = 0; k < 3; k++) {
                tinyobj::index_t idx = shapes[i].mesh.indices[3 * j + k];

                positions.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                positions.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                positions.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

                normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
                normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
                normals.push_back(attrib.normals[3 * idx.normal_index + 2]);

                tex_coords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                tex_coords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
            }
            material_id = shapes[i].mesh.material_ids[j];
        }

        Material material;
        material.ambient = vec3(materials[material_id].ambient);
        material.diffuse = vec3(materials[material_id].diffuse);
        material.specular = vec3(materials[material_id].specular);
        material.shininess = materials[material_id].shininess;
        material.diffuse_map = Texture::load_from_file(materials[material_id].diffuse_texname.c_str());
        material.draw_outline = true;

        Mesh mesh;
        mesh.num_vertices = shapes[i].mesh.num_face_vertices.size();
        mesh.material_id = add_material(material);

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        if (positions.size() > 0) {
            glGenBuffers(1, &mesh.position_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.position_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(0);
        }
        else {
            mesh.position_vbo = 0;
        }

        if (normals.size() > 0) {
            glGenBuffers(1, &mesh.normal_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.normal_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(1);
        }
        else {
            mesh.normal_vbo = 0;
        }

        if (tex_coords.size() > 0) {
            glGenBuffers(1, &mesh.tex_coord_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.tex_coord_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tex_coords.size(), tex_coords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(2);
        }
        else {
            mesh.tex_coord_vbo = 0;
        }

        mesh_ids->push_back(add_mesh(mesh));
    }
}

int Scene::add_mesh(Mesh mesh) {
    meshes.push_back(mesh);
    return meshes.size() - 1;
}

int Scene::add_instance(int mesh_id) {
    Transform transform;
    transforms.push_back(transform);

    Instance instance;
    instance.mesh_id = mesh_id;
    instance.transform_id = transforms.size() - 1;
    instances.push_back(instance);

    return instances.size() - 1;
}

int Scene::add_material(Material material) {
    materials.push_back(material);
    return materials.size() - 1;
};
