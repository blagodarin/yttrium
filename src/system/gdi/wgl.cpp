#include "wgl.h"

#include "../../renderer/gl/version.h"
#include "../gl.h"

#include <cstring>
#include <stdexcept>

namespace
{
	HGLRC create_hglrc(HDC hdc) noexcept
	{
		PIXELFORMATDESCRIPTOR pfd = { sizeof pfd };
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;
		const auto pf = ::ChoosePixelFormat(hdc, &pfd);
		if (!pf || !::SetPixelFormat(hdc, pf, &pfd))
			return nullptr;
		HGLRC hglrc = nullptr;
		const auto legacy_hglrc = ::wglCreateContext(hdc);
		if (legacy_hglrc)
		{
			::wglMakeCurrent(hdc, legacy_hglrc);
			const auto wglCreateContextAttribsARB = reinterpret_cast<HGLRC (APIENTRY*)(HDC, HGLRC, const int*)>(::wglGetProcAddress("wglCreateContextAttribsARB"));
			if (wglCreateContextAttribsARB)
			{
				const int attributes[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, Yttrium::Gl::required_major,
					WGL_CONTEXT_MINOR_VERSION_ARB, Yttrium::Gl::required_minor,
					WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					0
				};
				hglrc = wglCreateContextAttribsARB(hdc, nullptr, attributes);
			}
			::wglMakeCurrent(nullptr, nullptr);
			::wglDeleteContext(legacy_hglrc);
		}
		return hglrc;
	}

	bool check_extension(const char* list, const char* name)
	{
		const size_t name_size = ::strlen(name);
		while ((list = ::strstr(list, name)))
		{
			list += name_size;
			if (*list == ' ' || *list == '\0')
				return true;
		}
		return false;
	}

	template <typename T>
	void get_proc_address(T& address, const char* name)
	{
		address = reinterpret_cast<T>(::wglGetProcAddress(name));
	}
}

namespace Yttrium
{
	WglContext::WglContext(HDC hdc)
		: _hglrc(::create_hglrc(hdc))
	{
		if (!_hglrc)
			throw std::runtime_error("Failed to create OpenGL context");
		::wglMakeCurrent(hdc, _hglrc);
		::get_proc_address(_wgl.GetExtensionsStringARB, "wglGetExtensionsStringARB");
		if (_wgl.GetExtensionsStringARB)
		{
			_wgl.ARB_extensions_string = true;
			_wgl.EXTENSIONS_ARB = _wgl.GetExtensionsStringARB(hdc);
			if (_wgl.EXT_swap_control = ::check_extension(_wgl.EXTENSIONS_ARB, "WGL_EXT_swap_control"))
			{
				::get_proc_address(_wgl.GetSwapIntervalEXT, "wglGetSwapIntervalEXT");
				::get_proc_address(_wgl.SwapIntervalEXT, "wglSwapIntervalEXT");
			}
		}
	}

	WglContext::~WglContext()
	{
		::wglMakeCurrent(nullptr, nullptr);
		::wglDeleteContext(_hglrc);
	}

	GlAddress gl_address(const char* name)
	{
		auto p = ::wglGetProcAddress(name);
		if (!p || p == (void*)1 || p == (void*)2 || p == (void*)3 || p == (void*)-1) // Extra failure results from the OpenGL wiki.
			p = ::GetProcAddress(::GetModuleHandleA("opengl32.dll"), name);
		return reinterpret_cast<GlAddress>(p);
	}
}
