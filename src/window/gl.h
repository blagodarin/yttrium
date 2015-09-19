#ifndef _src_window_gl_h_
#define _src_window_gl_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "x11/gl.h"
#endif

namespace Yttrium
{
	class WindowBackend;

	namespace Gl
	{
		typedef void (__Y_GLAPI *Address)();

		Address address(WindowBackend& backend, const char* name);
	}
}

#endif
