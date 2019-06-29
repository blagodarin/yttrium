//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "wgl.h"

#include <yttrium/exceptions.h>
#include "../../renderer/_opengl/version.h"
#include "../opengl.h"

#include <cstring>

namespace
{
	bool check_extension(const char* list, const char* name)
	{
		const size_t name_size = ::strlen(name);
		for (;;)
		{
			list = std::strstr(list, name);
			if (!list)
				return false;
			list += name_size;
			if (*list == ' ' || *list == '\0')
				return true;
		}
	}

	template <typename T>
	void get_proc_address(T& address, const char* name)
	{
		address = reinterpret_cast<T>(::wglGetProcAddress(name));
	}
}

namespace Yttrium
{
	WglContext::RenderingContext::RenderingContext(HDC hdc, HGLRC hglrc)
		: _hglrc(hglrc)
	{
		if (_hglrc)
			::wglMakeCurrent(hdc, _hglrc);
	}

	WglContext::RenderingContext::~RenderingContext()
	{
		if (_hglrc)
		{
			::wglMakeCurrent(nullptr, nullptr);
			::wglDeleteContext(_hglrc);
		}
	}

	HGLRC WglContext::RenderingContext::create(HDC hdc)
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
			throw InitializationError("Failed to set pixel format");
		const RenderingContext legacy_context(hdc, ::wglCreateContext(hdc));
		if (!legacy_context)
			throw InitializationError("Unable to create OpenGL legacy context");
		const auto wglCreateContextAttribsARB = reinterpret_cast<HGLRC(APIENTRY*)(HDC, HGLRC, const int*)>(::wglGetProcAddress("wglCreateContextAttribsARB"));
		if (!wglCreateContextAttribsARB)
			throw InitializationError("Unable to use WGL_ARB_create_context");
		const int attributes[]{
			WGL_CONTEXT_MAJOR_VERSION_ARB, Gl::required_major,
			WGL_CONTEXT_MINOR_VERSION_ARB, Gl::required_minor,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};
		return wglCreateContextAttribsARB(hdc, nullptr, attributes);
	}

	WglContext::WglContext(HDC hdc)
		: _hglrc(hdc, RenderingContext::create(hdc))
	{
		if (!_hglrc)
			throw InitializationError("Unable to create OpenGL context");
		::get_proc_address(_wgl.GetExtensionsStringARB, "wglGetExtensionsStringARB");
		if (_wgl.GetExtensionsStringARB)
		{
			_wgl.ARB_extensions_string = true;
			_wgl.EXTENSIONS_ARB = _wgl.GetExtensionsStringARB(hdc);
			if (_wgl.EXT_swap_control = ::check_extension(_wgl.EXTENSIONS_ARB, "WGL_EXT_swap_control"); _wgl.EXT_swap_control)
			{
				::get_proc_address(_wgl.GetSwapIntervalEXT, "wglGetSwapIntervalEXT");
				::get_proc_address(_wgl.SwapIntervalEXT, "wglSwapIntervalEXT");
			}
		}
	}

	GlAddress gl_address(const char* name)
	{
		auto p = ::wglGetProcAddress(name);
		if (!p || p == (void*)1 || p == (void*)2 || p == (void*)3 || p == (void*)-1) // Extra failure results from the OpenGL wiki.
			p = ::GetProcAddress(::GetModuleHandleA("opengl32.dll"), name);
		return reinterpret_cast<GlAddress>(p);
	}
}
