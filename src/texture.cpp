#include "texture.h"

GLuint Texture::load_from_file(const char *file_name) {
    int x, y, n;
    int force_channels = 4;
    stbi_set_flip_vertically_on_load(0);
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

    if (!image_data) {
        printf("Could not load texture file %s\n", file_name);
        return 0;
    }

    GLuint location;
    glGenTextures(1, &location);
    glBindTexture(GL_TEXTURE_2D, location);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //GLfloat f_largest;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f_largest);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f_largest);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(image_data);

    return location;
}
