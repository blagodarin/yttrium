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
#include "application.h"

#if Y_RENDERER_OPENGL
#	include "wgl.h"
#endif

#include <optional>
#include <string>

namespace Yttrium
{
	class Point;
	class WindowBackendCallbacks;

	class WindowBackend : private NativeWindow
	{
	public:
		WindowBackend(const std::string& name, WindowBackendCallbacks&);

		void close();
		bool get_cursor(Point&);
		bool process_events() { return _application.process_events(); }
		bool set_cursor(const Point&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		LRESULT window_proc(HWND, UINT, WPARAM, LPARAM) override;

	private:
		class WindowHandle
		{
		public:
			WindowHandle(HWND hwnd) noexcept
				: _handle{ hwnd } {}
			~WindowHandle() noexcept { reset(); }
			void reset() noexcept;
			operator HWND() const { return _handle; }

		private:
			HWND _handle = NULL;
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
		NativeApplication _application;
		WindowHandle _hwnd{ _application.create_window(_name.c_str(), this) };
		const WindowDC _hdc{ _hwnd };
#if Y_RENDERER_OPENGL
		const WglContext _wgl{ _hdc };
#endif
	};
}
