// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/id.h>
#include <yttrium/math/size.h>
#include "application.h"
#include "glx.h"

#include <optional>
#include <string>

namespace Yt
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
		WindowID id() const noexcept { return { reinterpret_cast<intptr_t>(_application.display()), static_cast<intptr_t>(_window.get()) }; }
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
		primal::CPtr<std::remove_pointer_t<XIC>, ::XDestroyIC> _input_context;
		const EmptyCursor _empty_cursor{ _application.display(), _window.get() };
		::Atom _wm_protocols = ::XInternAtom(_application.display(), "WM_PROTOCOLS", False);
		::Atom _wm_delete_window = ::XInternAtom(_application.display(), "WM_DELETE_WINDOW", False);
		WindowBackendCallbacks& _callbacks;
	};
}
