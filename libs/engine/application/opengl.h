#pragma once

#ifdef _WIN32
#	define GLAPI_CALL __stdcall
#else
#	define GLAPI_CALL
#endif

namespace Yttrium
{
	using GlAddress = void(GLAPI_CALL*)();
	GlAddress gl_address(const char* name);
}
