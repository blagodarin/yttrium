/// \file
/// \brief

#ifndef _include_yttrium_renderer_gpu_program_h_
#define _include_yttrium_renderer_gpu_program_h_

#include <string>

namespace Yttrium
{
	class Matrix4;

	/// GPU program.
	class GpuProgram
	{
	public:
		virtual ~GpuProgram() = default;

		///
		virtual void set_uniform(const std::string& name, const Matrix4& value) = 0;
	};
}

#endif
