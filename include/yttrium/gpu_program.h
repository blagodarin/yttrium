/// \file
/// \brief

#ifndef _include_yttrium_gpu_program_h_
#define _include_yttrium_gpu_program_h_

namespace Yttrium
{
	class Matrix4;
	class StaticString;

	/// GPU program.
	class GpuProgram
	{
	public:

		virtual ~GpuProgram() = default;

		///
		virtual void set_uniform(const StaticString& name, const Matrix4& value) = 0;
	};
}

#endif
