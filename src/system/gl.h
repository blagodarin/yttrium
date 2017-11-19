#ifndef _src_system_gl_h_
#define _src_system_gl_h_

#include "../config.h"
#if Y_IS_WINDOWS
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
