#include "../gl.h"

#include <GL/glx.h>

namespace Yttrium
{
	namespace Gl
	{
		Address address(WindowBackend&, const char* name)
		{
			return reinterpret_cast<Address>(::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
		}
	}
}
