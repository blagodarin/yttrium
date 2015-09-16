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
		#define GLAPI_DYNAMIC_GL
		#include "api.h"
	};
}

#endif
