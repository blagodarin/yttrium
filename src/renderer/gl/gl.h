#ifndef _src_renderer_gl_gl_h_
#define _src_renderer_gl_gl_h_

#include "../../platform/gl.h"

#include <string_view>
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
		GlApi& operator=(const GlApi&) = delete;

	private:
		bool has_extension(std::string_view) const;
	};
}

#endif
