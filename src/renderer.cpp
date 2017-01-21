#include "renderer.h"

void Renderer::paint() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

    Camera *camera = &scene->camera;
    mat4 view_mat = mat4::look_at(camera->eye, camera->target, camera->up);
    mat4 proj_mat = mat4::perspective_projection(camera->fov, camera->aspect, camera->near, camera->far);

    glUseProgram(shader);

    glUniformMatrix4fv(1, 1, GL_TRUE, view_mat.m);
    glUniformMatrix4fv(2, 1, GL_TRUE, proj_mat.m);

    for (int i = 0; i < scene->instances.size(); i++) {
        Instance instance = scene->instances[i];
        Mesh mesh = scene->meshes[instance.mesh_id];
        Transform transform = scene->transforms[instance.transform_id];
        Material material = scene->materials[mesh.material_id];

        mat4 translation = mat4::translation(transform.translation);
        mat4 scale = mat4::scale(transform.scale);
        mat4 rotation = transform.orientation.get_matrix();

        mat4 model_mat = translation * rotation * scale;
        glUniformMatrix4fv(0, 1, GL_TRUE, model_mat.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map);

        glUniform3f(3, material.ambient.x, material.ambient.y, material.ambient.z);
        glUniform3f(4, material.diffuse.x, material.diffuse.y, material.diffuse.z);
        glUniform3f(5, material.specular.x, material.specular.y, material.specular.z);
        glUniform1f(6, material.shininess);

        glBindVertexArray(mesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.num_vertices);
        glBindVertexArray(0);
    }

    glUseProgram(0);
}

void Renderer::resize(int width, int height) {
    this->width = width;
    this->height = height;
    scene->camera.aspect = width / (float) height;
    glViewport(0, 0, width, height);
}
