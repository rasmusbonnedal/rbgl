#define RB_GL_WINDOW_IMPLEMENTATION
#include "rb_gl_window.h"

BOOL init_app_kit();

int main(int argc, char** argv)
{
	init_app_kit();
	while(1)
	{
		rbgl_poll_events();
	}
	return 0;
}

