#include "../gl.h"

#include <GL/glx.h>

namespace Yttrium
{
	GlAddress gl_address(const char* name)
	{
		return ::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
	}
}
