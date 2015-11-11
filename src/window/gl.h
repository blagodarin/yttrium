#ifndef _src_window_gl_h_
#define _src_window_gl_h_

#include <yttrium/global.h>

#if Y_PLATFORM_WINDOWS
	#error Not implemented.
#else
	#define __Y_GLAPI
#endif

namespace Yttrium
{
	using GlAddress = void(*)();
	GlAddress gl_address(const char* name);
}

#endif
