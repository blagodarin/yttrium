#include "gl.h"

namespace Yttrium
{
	void GlApi::initialize(WindowBackend& window)
	{
		Y_UNUSED(window);

		#define GLAPI_INIT window
		#define GLAPI_DYNAMIC_GL
		#include "api.h"
	}
}
