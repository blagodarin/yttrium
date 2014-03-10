#ifndef __RENDERER_GL_GL_H
#define __RENDERER_GL_GL_H

#include "../../terminal/gl.h"
#include "api.h"

namespace Yttrium
{

struct GlApi
{
	void initialize(Window *window);

	#define GLAPI_DECLARE
	#include "api.h"
};

} // namespace Yttrium

#endif // __RENDERER_GL_GL_H
