#ifndef _src_system_x11_gl_h_
#define _src_system_x11_gl_h_

#include "../../utils/unique_ptr.h"

#include <GL/glx.h>

namespace Yttrium
{
	class GlContext
	{
	public:
		struct Extensions
		{
			bool EXT_swap_control = false;
			void (*SwapIntervalEXT)(::Display*, ::GLXDrawable, int) = nullptr;

			bool EXT_swap_control_tear = false;
		};

		GlContext(::Display*, int screen);
		~GlContext();

		explicit operator bool() const { return _context; }

		void bind(::Window) const;
		void swap_buffers(::Window) const;
		void unbind() const;
		const ::XVisualInfo* visual_info() const { return _visual_info.get(); }

		const Extensions* operator->() const { return &_extensions; }

	private:
		::Display* const _display;
		const int _screen;
		Y_UNIQUE_PTR(::XVisualInfo, ::XFree) _visual_info;
		::GLXContext _context = nullptr;
		Extensions _extensions;
	};
}

#endif
