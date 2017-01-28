#ifndef _src_system_gdi_wgl_h_
#define _src_system_gdi_wgl_h_

#include "../windows/windows.h"

#include <GL/gl.h>
#include <GL/wglext.h>

namespace Yttrium
{
	class WglContext
	{
	public:
		struct Extensions
		{
			bool ARB_extensions_string = false;
			const char* (APIENTRY *GetExtensionsStringARB)(HDC) = nullptr;
			const char* EXTENSIONS_ARB = nullptr;

			bool EXT_swap_control = false;
			int (APIENTRY *GetSwapIntervalEXT)() = nullptr;
			BOOL (APIENTRY *SwapIntervalEXT)(int) = nullptr;
		};

		WglContext(HDC);
		~WglContext();

		const Extensions* operator->() const { return &_wgl; }

	private:
		const HGLRC _hglrc;
		Extensions _wgl;
	};
}

#endif
