#pragma once

#include <GL/glew.h>
#include <stdlib.h>

#include "stb_image.h"

class Texture {
    public:
        static GLuint load_from_file(const char *file_name);
};
