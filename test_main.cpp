#include "rb_gl_window.h"

#include <OpenGL/gl.h>

int main(int argc, char **argv)
{
    float r = 1;

    if (!rbgl_create_window(480, 480, "test_main_cpp", RBGL_CWF_NONE)) return -1;

    while (!rbgl_window_should_close())
    {
        rbgl_poll_events();
        glClearColor(r, 0, 0, 0);
        r += 0.01;
        if (r > 1) r = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        rbgl_swap_buffers();
    }
    rbgl_destroy_window();
    rbgl_app_terminate();
    return 0;
}
