#include "gl.h"

namespace Yttrium
{

void GlApi::initialize(Window *window)
{
	Y_UNUSED(window);

	#define GLAPI_INIT window
	#include "api.h"
}

} // namespace Yttrium
