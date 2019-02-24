//
// Copyright 2019 Sergei Blagodarin
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
#include "glx.h"

#include <optional>

namespace Yttrium
{
	class Image;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:
		explicit WindowBackend(WindowBackendCallbacks&);
		~WindowBackend() noexcept;

		void close() noexcept;
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void set_icon(const Image&);
		void set_title(const std::string&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		class WindowHandle
		{
		public:
			WindowHandle(::Display* display, ::Window window)
				: _display(display), _window(window) {}
			~WindowHandle() noexcept { reset(); }
			explicit operator bool() const noexcept { return _window != None; }
			::Window get() const noexcept { return _window; }
			void reset() noexcept;

		private:
			::Display* const _display;
			::Window _window = None;
		};

		class EmptyCursor
		{
		public:
			EmptyCursor(::Display*, ::Window);
			~EmptyCursor() noexcept;
			::Cursor get() const { return _cursor; }

		private:
			::Display* const _display;
			::Cursor _cursor = None;
		};

		std::optional<Size> _size;
		NativeApplication _application;
		const GlxContext _glx{ _application.display(), _application.screen() };
		WindowHandle _window;
		bool _pending_autorepeat = false;
		UniquePtr<std::remove_pointer_t<::XIC>, ::XDestroyIC> _input_context;
		const EmptyCursor _empty_cursor{ _application.display(), _window.get() };
		::Atom _wm_protocols = ::XInternAtom(_application.display(), "WM_PROTOCOLS", False);
		::Atom _wm_delete_window = ::XInternAtom(_application.display(), "WM_DELETE_WINDOW", False);
		WindowBackendCallbacks& _callbacks;
	};
}
