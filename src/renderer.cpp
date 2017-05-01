#include "renderer.h"

TextureViewer::TextureViewer() {
    GLuint points_vbo, texture_coords_vbo;

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texture_coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
}

void TextureViewer::draw_texture(GLuint texture) {
    glUniform3f(DIFFUSE_LOCATION, 1.0, 1.0, 1.0);
    glUniform1f(SINGLE_COLOR_LOCATION, false);
    glUniform1f(FOR_SHADOW_LOCATION, false);
    glUniform1f(FOR_UI_LOCATION, true);
    glUniform1f(ONLY_TEXTURE_LOCATION, true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(this->vao); 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Shadow::Shadow(int size) {
    this->size = size;

    glGenFramebuffers(1, &this->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fb);

    glGenTextures(1, &this->fb_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->fb_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->size, this->size, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->fb_tex, 0);
    GLenum draw_bufs[] = { GL_NONE };
    glDrawBuffers(1, draw_bufs);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::Renderer() : shadow(2048), shadow_2(2048) {
    shadow.view_mat = mat4::look_at(vec3(0.0, 10.0, 10.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    shadow.proj_mat = mat4::orthographic_projection(10.0, -10.0, 10.0, -10.0, 0.0, 30.0);

    shadow_2.view_mat = mat4::look_at(vec3(0.0, 10.0, 10.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    shadow_2.proj_mat = mat4::orthographic_projection(50.0, -50.0, 50.0, -50.0, -65.0, 65.0);

    {
        coordinate_system_mesh.num_vertices = 6;
        float points[18] = {
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
        };

        glGenVertexArrays(1, &coordinate_system_mesh.vao);
        glBindVertexArray(coordinate_system_mesh.vao);

        glGenBuffers(1, &coordinate_system_mesh.position_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, coordinate_system_mesh.position_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }

    {
        point_mesh.num_vertices = 1;
        float points[3] = {
            0.0, 0.0, 0.0,
        };

        glGenVertexArrays(1, &point_mesh.vao);
        glBindVertexArray(point_mesh.vao);

        glGenBuffers(1, &point_mesh.position_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, point_mesh.position_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }
}

void Renderer::create_shadow_map(Shadow *shadow) {
    glViewport(0, 0, shadow->size, shadow->size);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow->fb);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glUniformMatrix4fv(VIEW_MAT_LOCATION, 1, GL_TRUE, shadow->view_mat.m);
    glUniformMatrix4fv(PROJ_MAT_LOCATION, 1, GL_TRUE, shadow->proj_mat.m);

    for (int i = 0; i < scene->instances.size(); i++) {
        Instance instance = scene->instances[i];

        if (!instance.casts_shadow) {
            continue;
        }

        Mesh mesh = scene->meshes[instance.mesh_id];
        Transform transform = scene->transforms[instance.transform_id];
        Material material = scene->materials[mesh.material_id];

        mat4 translation = mat4::translation(transform.translation);
        mat4 scale = mat4::scale(transform.scale);
        mat4 rotation = transform.orientation.get_matrix();

        mat4 model_mat = translation * rotation * scale;
        glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, model_mat.m);
        glUniform1f(SINGLE_COLOR_LOCATION, false);
        glUniform1f(FOR_SHADOW_LOCATION, true);
        glUniform1f(FOR_UI_LOCATION, false);
        glUniform1f(ONLY_TEXTURE_LOCATION, false);

        glBindVertexArray(mesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.num_vertices);
        glBindVertexArray(0);
    }

    glViewport(0, 0, this->width, this->height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::paint() {
    glUseProgram(shader);

    create_shadow_map(&shadow);
    create_shadow_map(&shadow_2);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    Camera *camera = &scene->camera;

    glUniformMatrix4fv(VIEW_MAT_LOCATION, 1, GL_TRUE, camera->view_mat.m);
    glUniformMatrix4fv(PROJ_MAT_LOCATION, 1, GL_TRUE, camera->proj_mat.m);
    glUniformMatrix4fv(SHADOW_VIEW_MAT_LOCATION, 1, GL_TRUE, shadow.view_mat.m);
    glUniformMatrix4fv(SHADOW_PROJ_MAT_LOCATION, 1, GL_TRUE, shadow.proj_mat.m);
    glUniformMatrix4fv(SHADOW_2_VIEW_MAT_LOCATION, 1, GL_TRUE, shadow_2.view_mat.m);
    glUniformMatrix4fv(SHADOW_2_PROJ_MAT_LOCATION, 1, GL_TRUE, shadow_2.proj_mat.m);

    for (int i = 0; i < scene->instances.size(); i++) {
        Instance instance = scene->instances[i];
        Mesh mesh = scene->meshes[instance.mesh_id];
        Transform transform = scene->transforms[instance.transform_id];
        Material material = scene->materials[mesh.material_id];

        mat4 translation = mat4::translation(transform.translation);
        mat4 scale = mat4::scale(transform.scale);
        mat4 rotation = transform.orientation.get_matrix();

        mat4 model_mat = translation * rotation * scale;
        glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, model_mat.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadow.fb_tex);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, shadow_2.fb_tex);

        glUniform3f(AMBIENT_LOCATION, material.ambient.x, material.ambient.y, material.ambient.z);
        glUniform3f(DIFFUSE_LOCATION, material.diffuse.x, material.diffuse.y, material.diffuse.z);
        glUniform3f(SPECULAR_LOCATION, material.specular.x, material.specular.y, material.specular.z);
        glUniform1f(SHININESS_LOCATION, material.shininess);
        glUniform1f(SINGLE_COLOR_LOCATION, false);
        glUniform1f(FOR_SHADOW_LOCATION, false);
        glUniform1f(FOR_UI_LOCATION, false);
        glUniform1f(ONLY_TEXTURE_LOCATION, false);

        glBindVertexArray(mesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.num_vertices);

        if (instance.draw_outline) {
            glUniform3f(DIFFUSE_LOCATION, 1.0, 1.0, 1.0);
            glUniform1f(SINGLE_COLOR_LOCATION, true);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(2.0);
            glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.num_vertices);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glBindVertexArray(0);
    }

    for (int i = 0; i < scene->instances.size(); i++) {
        Instance instance = scene->instances[i];

        if (instance.draw_outline) {
            Mesh mesh = scene->meshes[instance.mesh_id];
            Transform transform = scene->transforms[instance.transform_id];
            mat4 translation = mat4::translation(transform.translation);
            mat4 coord_system_model_mat = translation * mat4::scale(vec3(1.0, 1.0, 1.0));
            mat4 ball_1_model_mat = translation * mat4::translation(vec3(1.0, 0.0, 0.0)) * mat4::scale(vec3(0.1, 0.1, 0.1));
            mat4 ball_2_model_mat = translation * mat4::translation(vec3(0.0, 1.0, 0.0)) * mat4::scale(vec3(0.1, 0.1, 0.1));
            mat4 ball_3_model_mat = translation * mat4::translation(vec3(0.0, 0.0, 1.0)) * mat4::scale(vec3(0.1, 0.1, 0.1));

            glDisable(GL_DEPTH_TEST);
            glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, coord_system_model_mat.m);
            glUniform1f(SINGLE_COLOR_LOCATION, true);
            glBindVertexArray(coordinate_system_mesh.vao);
            glUniform3f(DIFFUSE_LOCATION, 1.0, 0.0, 0.0);
            glDrawArrays(GL_LINES, 0, 2);
            glUniform3f(DIFFUSE_LOCATION, 0.0, 1.0, 0.0);
            glDrawArrays(GL_LINES, 2, 4);
            glUniform3f(DIFFUSE_LOCATION, 0.0, 0.0, 1.0);
            glDrawArrays(GL_LINES, 4, 6);

            glBindVertexArray(scene->meshes[scene->sphere_mesh_id].vao);

            glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, ball_1_model_mat.m);
            glUniform3f(DIFFUSE_LOCATION, 1.0, 0.0, 0.0);
            glDrawArrays(GL_TRIANGLES, 0, 3 * scene->meshes[scene->sphere_mesh_id].num_vertices);

            glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, ball_2_model_mat.m);
            glUniform3f(DIFFUSE_LOCATION, 0.0, 1.0, 0.0);
            glDrawArrays(GL_TRIANGLES, 0, 3 * scene->meshes[scene->sphere_mesh_id].num_vertices);

            glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, ball_3_model_mat.m);
            glUniform3f(DIFFUSE_LOCATION, 0.0, 0.0, 1.0);
            glDrawArrays(GL_TRIANGLES, 0, 3 * scene->meshes[scene->sphere_mesh_id].num_vertices);

            glEnable(GL_DEPTH_TEST);
        }
    }

    /*
    for (int i = 0; i < points_to_render.size(); i++) {
        vec3 p = points_to_render[i];

        glDisable(GL_DEPTH_TEST);
        glPointSize(5.0);
        glUniformMatrix4fv(MODEL_MAT_LOCATION, 1, GL_TRUE, mat4::translation(p).m);
        glUniform1f(SINGLE_COLOR_LOCATION, true);
        glUniform3f(DIFFUSE_LOCATION, 1.0, 1.0, 0.0);
        glBindVertexArray(point_mesh.vao);
        glDrawArrays(GL_POINTS, 0, 1);
        glEnable(GL_DEPTH_TEST);
    }
    */

    glUseProgram(0);
}

void Renderer::resize(int width, int height) {
    this->width = width;
    this->height = height;
    scene->camera.aspect = width / (float) height;
    glViewport(0, 0, width, height);
}
