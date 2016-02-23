#ifndef _src_window_x11_glx_h_
#define _src_window_x11_glx_h_

#include <GL/glx.h>

namespace Yttrium
{
	struct Glx
	{
		bool EXT_swap_control = false;
		bool EXT_swap_control_tear = false;

		void (*SwapIntervalEXT)(::Display*, ::GLXDrawable, int) = nullptr;

		Glx(::Display*, int screen);
	};
}

#endif
