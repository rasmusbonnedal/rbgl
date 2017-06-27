#define GLFW_INCLUDE_GLCOREARB 1
#include <GLFW/glfw3.h>

#include <OpenGL/gl3.h>
#include <stdio.h>

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

void load_shaders()
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLint result = GL_FALSE;

    char const* vertexShader = 
        "#version 330 core\n"
        "\n"
        "in vec3 in_Position;\n"
        "void main() {\n"
        "    gl_Position = vec4(in_Position, 1.0);\n"
        "    gl_Position.w = 1.0;\n"
        "}";
    char const* pixelShader = 
        "#version 330 core\n"
        "out vec3 color;\n"
        "void main() {\n"
        "    color = vec3(1,0,1);\n"
        "}";
    glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
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

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);
}

void error_callback(int error, const char* desc)
{
    printf("Error: %d %s\n", error, desc);
}

int main(int argc, char **argv)
{
    float r = 0.5;
    GLuint vertexbuffer;
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return -1;    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(480, 480, "test_main_c", 0, 0);
    if (window == 0) return -1;
    glfwMakeContextCurrent(window);
    printf("Renderer %s (%s)\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    load_shaders();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(r, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
