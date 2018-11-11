#pragma once

#include "../../application/opengl.h"

#include <string_view>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

namespace Yttrium
{
	class GlApi
	{
	public:
		#include "api.h"

		GlApi();

		GlApi(const GlApi&) = delete;
		GlApi& operator=(const GlApi&) = delete;

	private:
		bool has_extension(std::string_view) const;
	};
}
