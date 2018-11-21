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

#include "application.h"

#include "error.h"

#include <vector>

namespace Yttrium
{
	const char* const NativeApplication::WindowClass::Name = "Yttrium";

	NativeApplication::NativeApplication() = default;

	NativeApplication::~NativeApplication() noexcept = default;

	HWND NativeApplication::create_window(const char* title, NativeWindow* window)
	{
		const auto hwnd = ::CreateWindowExA(WS_EX_APPWINDOW, WindowClass::Name, title, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, _hinstance, window);
		if (!hwnd)
			throw_last_error("CreateWindowEx");
		return hwnd;
	}

	bool NativeApplication::process_events()
	{
		// TODO: Process VK_SNAPSHOT, VK_{L,R}SHIFT, VK_{L,R}CONTROL, VK_{L,R}MENU.
		MSG msg;
		while (::PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!::GetMessageA(&msg, NULL, 0, 0))
				return false;
			::TranslateMessage(&msg);
			::DispatchMessageA(&msg);
		}
		return true;
	}

	LRESULT NativeApplication::static_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		NativeWindow* window = nullptr;
		if (msg == WM_NCCREATE)
		{
			window = static_cast<NativeWindow*>(((CREATESTRUCTA*)lparam)->lpCreateParams);
			::SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		}
		else
			window = (NativeWindow*)::GetWindowLongPtrA(hwnd, GWLP_USERDATA);
		return window->window_proc(hwnd, msg, wparam, lparam);
	}

	NativeApplication::EmptyCursor::EmptyCursor(HINSTANCE hinstance)
	{
		const auto width = ::GetSystemMetrics(SM_CXCURSOR);
		const auto height = ::GetSystemMetrics(SM_CYCURSOR);
		std::vector<std::uint8_t> and_mask(width * height / 8, 0xff);
		std::vector<std::uint8_t> xor_mask(width * height / 8, 0x00);
		_handle = ::CreateCursor(hinstance, 0, 0, width, height, and_mask.data(), xor_mask.data());
		if (!_handle)
			throw_last_error("CreateCursor");
	}

	NativeApplication::EmptyCursor::~EmptyCursor() noexcept
	{
		if (!::DestroyCursor(_handle))
			print_last_error("DestroyCursor");
	}

	NativeApplication::WindowClass::WindowClass(HINSTANCE hinstance, HCURSOR hcursor, WNDPROC wndproc)
		: _hinstance{ hinstance }
	{
		WNDCLASSEXA wndclass{ sizeof wndclass };
		wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		wndclass.lpfnWndProc = wndproc;
		wndclass.hInstance = _hinstance;
		wndclass.hIcon = ::LoadIconA(nullptr, IDI_APPLICATION);
		wndclass.hCursor = hcursor;
		wndclass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wndclass.lpszClassName = Name;
		if (!::RegisterClassExA(&wndclass))
			throw_last_error("RegisterClassEx");
	}

	NativeApplication::WindowClass::~WindowClass()
	{
		if (!::UnregisterClassA(Name, _hinstance))
			print_last_error("UnregisterClass");
	}
}
