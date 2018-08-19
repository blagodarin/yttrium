#ifndef _src_renderer_builtin_builtin_h_
#define _src_renderer_builtin_builtin_h_

#include <memory>

namespace Yttrium
{
	class RenderBackend;
	class RenderProgram;
	class Texture2D;

	class RenderBuiltin
	{
	public:
		const std::unique_ptr<const Texture2D> _white_texture;
		const std::unique_ptr<const Texture2D> _debug_texture;
		const std::unique_ptr<RenderProgram> _program_2d;

		explicit RenderBuiltin(RenderBackend&);
		~RenderBuiltin() noexcept;
	};
}

#endif
