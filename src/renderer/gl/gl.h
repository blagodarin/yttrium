#ifndef _src_renderer_gl_gl_h_
#define _src_renderer_gl_gl_h_

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
