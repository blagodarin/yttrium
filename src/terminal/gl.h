#ifndef __TERMINAL_GL_H
#define __TERMINAL_GL_H

#include <yttrium/global.h>

#if Y_IS_POSIX
	#include "x11/gl.h"
#elif Y_IS_WINDOWS
	#include "windows/gl.h"
#endif

namespace Yttrium
{

class Window;

namespace Gl
{

typedef void (__Y_GLAPI *Address)();

Address address(Window *window, const char *name);

} // namespace Gl

} // namespace Yttrium

#endif // __TERMINAL_GL_H
