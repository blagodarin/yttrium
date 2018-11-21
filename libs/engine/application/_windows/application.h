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

#include <windows.h>

namespace Yttrium
{
	class NativeWindow
	{
	public:
		virtual ~NativeWindow() = default;
		virtual LRESULT window_proc(HWND, UINT, WPARAM, LPARAM) = 0;
	};

	class NativeApplication
	{
	public:
		NativeApplication();
		~NativeApplication() noexcept;

		HWND create_window(const char* title, NativeWindow*);
		bool process_events();

	private:
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

			static const char* const Name;

		private:
			const HINSTANCE _hinstance;
		};

		const HINSTANCE _hinstance = ::GetModuleHandleA(nullptr);
		const EmptyCursor _cursor{ _hinstance };
		const WindowClass _window_class{ _hinstance, _cursor, static_window_proc };
	};
}
