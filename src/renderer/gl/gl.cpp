#include "gl.h"

#include <yttrium/log.h>
#include "version.h"

#include <cassert>
#include <stdexcept>

namespace Yttrium
{
	GlApi::GlApi(Allocator& allocator)
		: MAX_VIEWPORT_DIMS(allocator)
		, COMPRESSED_TEXTURE_FORMATS(allocator)
		, EXTENSIONS(allocator)
	{
		#define GLAPI_GET_FUNCTION(name) gl_address(name)
		#define GLAPI_HAS_EXTENSION(name) has_extension(name)
		#include "api.h"

		assert(MAJOR_VERSION >= Gl::required_major
			|| (MAJOR_VERSION == Gl::required_major && MINOR_VERSION < Gl::required_minor));

		Log() << "Renderer backend: OpenGL " << VERSION;

		if (!ARB_vertex_attrib_binding)
			throw std::runtime_error("GL_ARB_vertex_attrib_binding is unavailable");

		if (!EXT_direct_state_access)
			throw std::runtime_error("GL_EXT_direct_state_access is unavailable");

	#if Y_IS_DEBUG
		Log() << "GL_MAX_3D_TEXTURE_SIZE = " << MAX_3D_TEXTURE_SIZE;
		Log() << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << MAX_COMBINED_TEXTURE_IMAGE_UNITS;
		Log() << "GL_MAX_ELEMENTS_INDICES = " << MAX_ELEMENTS_INDICES;
		Log() << "GL_MAX_ELEMENTS_VERTICES = " << MAX_ELEMENTS_VERTICES;
		if (EXT_texture_filter_anisotropic)
			Log() << "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = " << MAX_TEXTURE_MAX_ANISOTROPY_EXT;
		Log() << "GL_MAX_TEXTURE_SIZE = " << MAX_TEXTURE_SIZE;
		Log() << "GL_MAX_VIEWPORT_DIMS = {" << MAX_VIEWPORT_DIMS[0] << ", " << MAX_VIEWPORT_DIMS[1] << "}";
		Log() << "GL_RENDERER = \"" << RENDERER << "\"";
		Log() << "GL_VENDOR = \"" << VENDOR << "\"";
	#endif
	}

	bool GlApi::has_extension(const char* name) const
	{
		for (const auto& extension : EXTENSIONS)
			if (extension == name)
				return true;
		return false;
	}
}
