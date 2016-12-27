#include "shader.h"

GLuint Shader::load_from_file(const char *fragment_shader_file_name, const char *vertex_shader_file_name) {
    GLuint program;

    // Create the vertex shader
    FILE *f = fopen(vertex_shader_file_name, "r");
    fseek(f, 0, SEEK_END);
    int l = ftell(f); 
    fseek(f, 0, SEEK_SET);
    char *vertex_shader_string = (char*) malloc(sizeof(char) * (l + 1));
    fread(vertex_shader_string, sizeof(char), l, f);
    vertex_shader_string[l] = 0;
    fclose(f);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_string, NULL);
    glCompileShader(vertex_shader);
    Shader::check_for_compile_errors(vertex_shader, vertex_shader_file_name);
    free(vertex_shader_string);

    // Create the fragment shader
    f = fopen(fragment_shader_file_name, "r");
    fseek(f, 0, SEEK_END);
    l = ftell(f); 
    fseek(f, 0, SEEK_SET);
    char *fragment_shader_string = (char*) malloc(sizeof(char) * (l + 1));
    fread(fragment_shader_string, sizeof(char), l, f);
    fragment_shader_string[l] = 0;
    fclose(f);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_string, NULL);
    glCompileShader(fragment_shader);
    Shader::check_for_compile_errors(fragment_shader, fragment_shader_file_name);
    free(fragment_shader_string);

    // Create the shader program
    program = glCreateProgram();
    glAttachShader(program, fragment_shader);
    glAttachShader(program, vertex_shader);
    glLinkProgram(program);

    return program;
}

void Shader::check_for_compile_errors(GLuint shader, const char *name) {
    int params = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE) {
        char *log = (char*) malloc(sizeof(char)*2048);
        int length;
        glGetShaderInfoLog(shader, 2048, &length, log);

        printf("Could not compile shader: %s\n", name);
        printf("Shader log:\n%s\n", log);
    }
}
