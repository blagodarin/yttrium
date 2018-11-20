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

#pragma once

#include <yttrium/math/size.h>

#if Y_RENDERER_OPENGL
#	include "wgl.h"
#endif

#include <optional>
#include <string>

#include <windows.h>

namespace Yttrium
{
	class Point;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:
		WindowBackend(const std::string& name, WindowBackendCallbacks&);

		void close();
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		LRESULT window_proc(HWND, UINT, WPARAM, LPARAM);

		static LRESULT CALLBACK static_window_proc(HWND, UINT, WPARAM, LPARAM);

	private:
		class EmptyCursor
		{
		public:
			explicit EmptyCursor(HINSTANCE);
			~EmptyCursor() noexcept;
			operator HCURSOR() const noexcept { return _handle; }

		private:
			HCURSOR _handle = NULL;
		};

		class WindowClass
		{
		public:
			WindowClass(HINSTANCE, WNDPROC);
			~WindowClass() noexcept;
			HINSTANCE hinstance() const noexcept { return _hinstance; }
			const char* name() const noexcept { return _wndclass.lpszClassName; }

		private:
			const HINSTANCE _hinstance;
			const EmptyCursor _empty_cursor{ _hinstance };
			WNDCLASSEXA _wndclass = { sizeof _wndclass };
		};

		class WindowHandle
		{
		public:
			WindowHandle(const WindowClass&, const char* title, void* user_data);
			~WindowHandle() noexcept { reset(); }
			void reset() noexcept;
			operator HWND() const { return _hwnd; }

		private:
			HWND _hwnd = NULL;
			HDC _hdc = NULL;
		};

		class WindowDC
		{
		public:
			explicit WindowDC(HWND);
			operator HDC() const noexcept { return _hdc; }

		private:
			const HDC _hdc;
		};

		const std::string _name;
		WindowBackendCallbacks& _callbacks;
		bool _created = false;
		std::optional<Size> _size;
		const HINSTANCE _hinstance = ::GetModuleHandleA(nullptr);
		const WindowClass _wndclass{ _hinstance, reinterpret_cast<WNDPROC>(static_window_proc) };
		WindowHandle _hwnd{ _wndclass, _name.c_str(), this };
		const WindowDC _hdc{ _hwnd };
#if Y_RENDERER_OPENGL
		const WglContext _wgl{ _hdc };
#endif
	};
}
