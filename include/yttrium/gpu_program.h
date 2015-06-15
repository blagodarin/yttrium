/// \file
/// \brief

#ifndef __Y_GPU_PROGRAM_H
#define __Y_GPU_PROGRAM_H

namespace Yttrium
{
	class StaticString;

	/// GPU program.
	class GpuProgram
	{
	public:

		///
		enum class Language
		{
			Glsl, ///<
		};

		virtual ~GpuProgram() = default;

		///
		virtual bool is_linked() const = 0;

		///
		virtual bool link() = 0;

		///
		virtual bool set_fragment_shader(Language language, const StaticString& source) = 0;

		///
		virtual bool set_vertex_shader(Language language, const StaticString& source) = 0;
	};
}

#endif
