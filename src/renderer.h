#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"

class Renderer {
    private:

    public:
        int width, height;
        Scene *scene;
        GLuint shader;

        void paint();
        void resize(int width, int height);
};
