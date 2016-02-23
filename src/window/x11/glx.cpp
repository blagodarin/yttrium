#include "glx.h"

#include <cstring>

namespace Yttrium
{
	namespace
	{
		bool check_extension(const char* list, const char* name)
		{
			const size_t name_size = ::strlen(name);
			while ((list = ::strstr(list, name)))
			{
				list += name_size;
				if (*list == ' ' || *list == 0)
					return true;
			}
			return false;
		}

		template <typename T>
		void get_proc_address(T& address, const char* name)
		{
			address = reinterpret_cast<T>(::glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
		}
	}

	Glx::Glx(::Display* display, int screen)
	{
		const auto glx_extensions = ::glXQueryExtensionsString(display, screen);

		EXT_swap_control = check_extension(glx_extensions, "GLX_EXT_swap_control");
		if (EXT_swap_control)
			get_proc_address(SwapIntervalEXT, "glXSwapIntervalEXT");

		EXT_swap_control_tear = check_extension(glx_extensions, "GLX_EXT_swap_control_tear");
	}
}
