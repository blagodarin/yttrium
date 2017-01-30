#include "gl.h"

#include <yttrium/exceptions.h>
#include "version.h"

#include <cassert>
#include <iostream>

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

#ifndef NDEBUG
		std::cerr << "Renderer backend: OpenGL " << VERSION << "\n";
#endif

		if (!ARB_vertex_attrib_binding)
			throw InitializationError("GL_ARB_vertex_attrib_binding is unavailable");

		if (!EXT_direct_state_access)
			throw InitializationError("GL_EXT_direct_state_access is unavailable");

#ifndef NDEBUG
		std::cerr << "GL_MAX_3D_TEXTURE_SIZE = " << MAX_3D_TEXTURE_SIZE << "\n";
		std::cerr << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << MAX_COMBINED_TEXTURE_IMAGE_UNITS << "\n";
		std::cerr << "GL_MAX_ELEMENTS_INDICES = " << MAX_ELEMENTS_INDICES << "\n";
		std::cerr << "GL_MAX_ELEMENTS_VERTICES = " << MAX_ELEMENTS_VERTICES << "\n";
		if (EXT_texture_filter_anisotropic)
			std::cerr << "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = " << MAX_TEXTURE_MAX_ANISOTROPY_EXT << "\n";
		std::cerr << "GL_MAX_TEXTURE_SIZE = " << MAX_TEXTURE_SIZE << "\n";
		std::cerr << "GL_MAX_VIEWPORT_DIMS = {" << MAX_VIEWPORT_DIMS[0] << ", " << MAX_VIEWPORT_DIMS[1] << "}\n";
		std::cerr << "GL_RENDERER = \"" << RENDERER << "\"\n";
		std::cerr << "GL_VENDOR = \"" << VENDOR << "\"\n";
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
