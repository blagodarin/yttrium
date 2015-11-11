#ifndef _src_renderer_gl_gl_h_
#define _src_renderer_gl_gl_h_

#include <yttrium/std/vector.h>
#include "../../window/gl.h"

#include <GL/gl.h>
#include <GL/glext.h>

namespace Yttrium
{
	class StaticString;

	class GlApi
	{
	public:

		#include "api.h"

		GlApi(Allocator&);

		GlApi(const GlApi&) = delete;
		GlApi(GlApi&&) = delete;
		GlApi& operator=(const GlApi&) = delete;
		GlApi& operator=(GlApi&&) = delete;

	private:

		bool has_extension(const char* name) const;
	};
}

#endif
