#include "../gl.h"

#include <GL/glx.h>

// TODO: Move all GLX stuff here and rename this file to 'glx.cpp'.

namespace Yttrium
{
	GlAddress gl_address(const char* name)
	{
		return ::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
	}
}
