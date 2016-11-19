#ifndef _src_system_gl_h_
#define _src_system_gl_h_

#include <yttrium/global.h>

#if Y_PLATFORM_WINDOWS
	#error Not implemented.
#else
	#define GLAPI_CALL
#endif

namespace Yttrium
{
	using GlAddress = void(GLAPI_CALL *)();
	GlAddress gl_address(const char* name);
}

#endif
