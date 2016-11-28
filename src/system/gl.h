#ifndef _src_system_gl_h_
#define _src_system_gl_h_

#include "../config.h"
#ifdef Y_IS_WINDOWS
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
