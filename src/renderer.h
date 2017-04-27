#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "maths.h"
#include "shaders/preamble.glsl"

class Shadow {
    public:
        int size;
        GLuint fb, fb_tex;
        mat4 view_mat, proj_mat;

        Shadow(int size);        
};

class TextureViewer {
    private:
        float points[18] = {
            1.0, 1.0, 0.0,
            0.5, 1.0, 0.0,
            1.0, 0.5, 0.0,
            0.5, 1.0, 0.0,
            0.5, 0.5, 0.0,
            1.0, 0.5, 0.0,
        };

        float texture_coords[12] = {
            1.0, 0.0,
            0.0, 0.0,
            1.0, 1.0,
            0.0, 0.0,
            0.0, 1.0,
            1.0, 1.0,
        };

        GLuint vao;

    public:
        TextureViewer();
        void draw_texture(GLuint texture);
};

class Renderer {
    private:
        void create_shadow_map(Shadow *shadow);
        Shadow shadow;
        Shadow shadow_2;
        TextureViewer texture_viewer;
        Mesh coordinate_system_mesh;

    public:
        int width, height;
        Scene *scene;
        GLuint shader;

        void paint();
        void resize(int width, int height);
        void update();

        Renderer(); 
};
