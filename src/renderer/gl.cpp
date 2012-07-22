#include "gl.h"

namespace Yttrium
{

void GlApi::init(Window *window)
{
#	define GLAPI_INIT window
#	include "api.h"
}

} // namespace Yttrium
