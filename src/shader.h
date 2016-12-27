#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

class Shader {
    private:
        static void check_for_compile_errors(GLuint shader, const char *name);

    public:
        static GLuint load_from_file(const char *fragment_shader_file_name, const char *vertex_shader_file_name);
};
