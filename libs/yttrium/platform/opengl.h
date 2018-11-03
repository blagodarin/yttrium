#pragma once

#if defined(_WIN32)
	#include "_windows/api.h"
	#define GLAPI_CALL APIENTRY
#else
	#define GLAPI_CALL
#endif

namespace Yttrium
{
	using GlAddress = void(GLAPI_CALL *)();
	GlAddress gl_address(const char* name);
}
