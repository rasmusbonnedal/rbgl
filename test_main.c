#include "rb_gl_window.h"

#include <OpenGL/gl3.h>
#include <stdio.h>

void load_shaders()
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLint result = GL_FALSE;
    GLuint program_id;

    char const* vertexShaderTri = 
        "#version 330 core\n"
        "\n"
        "void main() {\n"
        "    float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
        "    float y = -1.0 + float((gl_VertexID & 2) << 1);\n"
        "    gl_Position = vec4(x * 0.1, y * 0.1, 0, 1);\n"
        "}";

    char const* pixelShader = 
        "#version 330 core\n"
        "out vec3 color;\n"
        "void main() {\n"
        "    color = vec3(1,0,1);\n"
        "}";

    glShaderSource(vertexShaderID, 1, &vertexShaderTri, NULL);
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    if (!result) {
        printf("Error compiling vertex shader\n");
    }

    glShaderSource(fragmentShaderID, 1, &pixelShader, NULL);
    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    if (!result) {
        printf("Error compiling fragment shader\n");
    }

    program_id = glCreateProgram();
    glAttachShader(program_id, vertexShaderID);
    glAttachShader(program_id, fragmentShaderID);
    glLinkProgram(program_id);
    glUseProgram(program_id);
}

int main(int argc, char **argv)
{
    float r = 0;
    GLuint vertex_array_id;
    int width, height;

    if (!rbgl_create_window(640, 480, "test_main_c", RBGL_CWF_CORE_32)) return -1;

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    load_shaders();

    while (!rbgl_window_should_close())
    {
        rbgl_make_context_current();
        rbgl_poll_events();
        rbgl_get_window_size(&width, &height);
        glViewport(0, 0, width, height);
        glClearColor(r, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        rbgl_swap_buffers();
    }
    rbgl_destroy_window();
    rbgl_app_terminate();
    return 0;
}
