#ifndef _src_platform_gl_h_
#define _src_platform_gl_h_

#if defined(_WIN32)
	#include "windows/windows.h"
	#define GLAPI_CALL APIENTRY
#else
	#define GLAPI_CALL
#endif

namespace Yttrium
{
	using GlAddress = void(GLAPI_CALL *)();
	GlAddress gl_address(const char* name);
}

#endif
