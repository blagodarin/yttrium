// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/event.h>

#include <seir_base/pointer.hpp>

#include <string_view>
#include <unordered_map>

#include <windows.h>

namespace Yt
{
	class Size;

	class NativeWindowCallbacks
	{
	public:
		virtual ~NativeWindowCallbacks() = default;
		virtual void on_close() = 0;
		virtual void on_focus(bool) = 0;
		virtual void on_key(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) = 0;
		virtual void on_resize(const Size&) = 0;
		virtual void on_text(std::string_view) = 0;
	};

	struct HWndDeleter
	{
		static void free(HWND) noexcept;
	};

	using NativeWindow = seir::Pointer<std::remove_pointer_t<HWND>, HWndDeleter>;

	class NativeApplication
	{
	public:
		NativeApplication();
		~NativeApplication() noexcept;

		NativeWindow create_window(NativeWindowCallbacks&);
		HINSTANCE instance() const noexcept { return _hinstance; }
		bool process_events();

	private:
		NativeWindowCallbacks* find_callbacks(HWND) const noexcept;
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
			WindowClass(HINSTANCE, HCURSOR, WNDPROC);
			~WindowClass() noexcept;

			static const wchar_t* const Name;

		private:
			const HINSTANCE _hinstance;
		};

		const HINSTANCE _hinstance = ::GetModuleHandleW(nullptr);
		const EmptyCursor _cursor{ _hinstance };
		const WindowClass _window_class{ _hinstance, _cursor, static_window_proc };
		std::unordered_map<HWND, NativeWindowCallbacks*> _windows;
	};
}
