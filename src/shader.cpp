#include "shader.h"

GLuint Shader::load_from_file(const char *preamble_file_name, 
        const char *fragment_shader_file_name,
        const char *vertex_shader_file_name) {
    GLuint program;

    const char *version_string = "#version 450\n";
    int version_string_length = strlen(version_string);

    FILE *preamble_file, *vertex_file, *fragment_file;
    int preamble_file_length, vertex_file_length, fragment_file_length;
    char *preamble_shader_string, *vertex_shader_string, *fragment_shader_string;

    // Get the string for the preamble file
    preamble_file = fopen(preamble_file_name, "r");
    fseek(preamble_file, 0, SEEK_END);
    preamble_file_length = ftell(preamble_file); 
    fseek(preamble_file, 0, SEEK_SET);
    preamble_shader_string = (char*) malloc(sizeof(char) * (preamble_file_length + 1));
    fread(preamble_shader_string, sizeof(char), preamble_file_length, preamble_file);
    preamble_shader_string[preamble_file_length] = 0;
    fclose(preamble_file);

    // Create the vertex shader
    vertex_file = fopen(vertex_shader_file_name, "r");
    fseek(vertex_file, 0, SEEK_END);
    vertex_file_length = ftell(vertex_file); 
    fseek(vertex_file, 0, SEEK_SET);
    vertex_shader_string = (char*) malloc(sizeof(char) * (version_string_length + preamble_file_length + vertex_file_length + 1));
    strcpy(vertex_shader_string, version_string);
    strcpy(vertex_shader_string + version_string_length, preamble_shader_string);
    fread(vertex_shader_string + version_string_length + preamble_file_length, sizeof(char), vertex_file_length, vertex_file);
    vertex_shader_string[version_string_length + preamble_file_length + vertex_file_length] = 0;
    fclose(vertex_file);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_string, NULL);
    glCompileShader(vertex_shader);
    Shader::check_for_compile_errors(vertex_shader, vertex_shader_file_name);
    free(vertex_shader_string);

    // Create the fragment shader
    fragment_file = fopen(fragment_shader_file_name, "r");
    fseek(fragment_file, 0, SEEK_END);
    fragment_file_length = ftell(fragment_file); 
    fseek(fragment_file, 0, SEEK_SET);
    fragment_shader_string = (char*) malloc(sizeof(char) * (version_string_length + preamble_file_length + fragment_file_length + 1));
    strcpy(fragment_shader_string, version_string);
    strcpy(fragment_shader_string + version_string_length, preamble_shader_string);
    fread(fragment_shader_string + version_string_length + preamble_file_length, sizeof(char), fragment_file_length, fragment_file);
    fragment_shader_string[version_string_length + preamble_file_length + fragment_file_length] = 0;
    fclose(fragment_file);

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
