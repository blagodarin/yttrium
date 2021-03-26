// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "gl.h"

#include <yttrium/base/exceptions.h>
#include <yttrium/base/logger.h>
#include "version.h"

#include <algorithm>
#include <cassert>

#include <fmt/format.h>

namespace Yt
{
	GlApi::GlApi()
	{
#define GLAPI_GET_FUNCTION(name) gl_address(name)
#define GLAPI_HAS_EXTENSION(name) has_extension(name)
#include "api.h"

		assert(MAJOR_VERSION >= Gl::required_major
			|| (MAJOR_VERSION == Gl::required_major && MINOR_VERSION < Gl::required_minor));

		Logger::write("OpenGL information:");
		Logger::write(fmt::format("  GL_VERSION = \"{}\"", VERSION));
		Logger::write(fmt::format("  GL_RENDERER = \"{}\"", RENDERER));
		Logger::write(fmt::format("  GL_VENDOR = \"{}\"", VENDOR));
		Logger::write(fmt::format("  GL_MAX_3D_TEXTURE_SIZE = {}", MAX_3D_TEXTURE_SIZE));
		Logger::write(fmt::format("  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = {}", MAX_COMBINED_TEXTURE_IMAGE_UNITS));
		Logger::write(fmt::format("  GL_MAX_ELEMENTS_INDICES = {}", MAX_ELEMENTS_INDICES));
		Logger::write(fmt::format("  GL_MAX_ELEMENTS_VERTICES = {}", MAX_ELEMENTS_VERTICES));
		if (EXT_texture_filter_anisotropic)
			Logger::write(fmt::format("  GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = {}", MAX_TEXTURE_MAX_ANISOTROPY_EXT));
		Logger::write(fmt::format("  GL_MAX_TEXTURE_SIZE = {}", MAX_TEXTURE_SIZE));
		Logger::write(fmt::format("  GL_MAX_VIEWPORT_DIMS = ( {}, {} )", MAX_VIEWPORT_DIMS[0], MAX_VIEWPORT_DIMS[1]));

		if (!ARB_vertex_attrib_binding)
			throw InitializationError{ "GL_ARB_vertex_attrib_binding is unavailable" };

		if (!EXT_direct_state_access)
			throw InitializationError{ "GL_EXT_direct_state_access is unavailable" };
	}

	bool GlApi::has_extension(std::string_view name) const
	{
		return std::find(std::begin(EXTENSIONS), std::end(EXTENSIONS), name) != std::end(EXTENSIONS);
	}
}
