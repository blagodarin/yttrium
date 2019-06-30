//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/key.h>

#include <string_view>
#include <unordered_map>

#include <windows.h>

namespace Yttrium
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

	class NativeWindow
	{
	public:
		explicit NativeWindow(HWND handle) noexcept
			: _handle{ handle } {}
		NativeWindow(const NativeWindow&) = delete;
		NativeWindow(NativeWindow&& other) noexcept
			: _handle{ other._handle } { other._handle = NULL; }
		~NativeWindow() noexcept { reset(); }
		NativeWindow& operator=(const NativeWindow&) = delete;
		void reset() noexcept;
		operator HWND() const noexcept { return _handle; }

	private:
		HWND _handle = NULL;
	};

	class NativeApplication
	{
	public:
		NativeApplication();
		~NativeApplication() noexcept;

		NativeWindow create_window(NativeWindowCallbacks&);
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
