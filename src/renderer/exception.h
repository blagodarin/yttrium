#ifndef _src_renderer_exception_h_
#define _src_renderer_exception_h_

#include <stdexcept>

namespace Yttrium
{
	class RendererError : public std::runtime_error
	{
	public:
		explicit RendererError(const char* what) : std::runtime_error(what) {}
	};
}

#endif
