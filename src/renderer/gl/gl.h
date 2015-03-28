#ifndef __RENDERER_GL_GL_H
#define __RENDERER_GL_GL_H

#include "../../window/gl.h"
#include "api.h"

namespace Yttrium
{
	struct GlApi
	{
		void initialize(WindowBackend& window);

		#define GLAPI_DECLARE
		#include "api.h"
	};
}

#endif // __RENDERER_GL_GL_H
