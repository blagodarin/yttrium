#ifndef __WINDOW_GL_H
#define __WINDOW_GL_H

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

#endif // __WINDOW_GL_H
