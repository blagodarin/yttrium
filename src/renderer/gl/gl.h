#ifndef _src_renderer_gl_gl_h_
#define _src_renderer_gl_gl_h_

#include "../../system/gl.h"

#include <string>
#include <vector>

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
		GlApi(GlApi&&) = delete;
		GlApi& operator=(const GlApi&) = delete;
		GlApi& operator=(GlApi&&) = delete;

	private:
		bool has_extension(const char*) const;
	};
}

#endif
