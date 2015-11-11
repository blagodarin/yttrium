#include "gl.h"

#include <yttrium/log.h>
#include "../exception.h"
#include "version.h"

#include <cassert>

namespace Yttrium
{
	GlApi::GlApi(Allocator& allocator)
		: EXTENSIONS(allocator)
	{
		#define GLAPI_GET_FUNCTION(name) gl_address(name)
		#define GLAPI_HAS_EXTENSION(name) has_extension(name)
		#include "api.h"

		assert(MAJOR_VERSION >= Gl::required_major
			|| (MAJOR_VERSION == Gl::required_major && MINOR_VERSION < Gl::required_minor));

		Log() << "Renderer backend: OpenGL " << VERSION;

		if (!EXT_direct_state_access)
			throw RendererError("OpenGL EXT_direct_state_access extension must be present");
	}

	bool GlApi::has_extension(const char* name) const
	{
		for (const auto& extension : EXTENSIONS)
			if (extension == name)
				return true;
		return false;
	}
}
