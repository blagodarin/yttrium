#ifndef _include_yttrium_renderer_program_h_
#define _include_yttrium_renderer_program_h_

#include <string>

namespace Yttrium
{
	class Matrix4;

	/// Rendering pipeline program.
	class RenderProgram
	{
	public:
		virtual ~RenderProgram() = default;

		///
		virtual void set_uniform(const std::string& name, const Matrix4&) = 0;
	};
}

#endif
