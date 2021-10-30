// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "application.h"

#include <yttrium/geometry/size.h>
#include <yttrium/base/numeric.h>
#include "../../../base/src/windows/error.h"

#include <primal/utf8.hpp>

#include <array>
#include <cassert>
#include <vector>

namespace Yt
{
	const wchar_t* const NativeApplication::WindowClass::Name = L"Yttrium";

	void HWndDeleter::free(HWND handle) noexcept
	{
		if (handle && !::DestroyWindow(handle))
			log_last_error("DestroyWindow");
	}

	NativeApplication::NativeApplication() = default;

	NativeApplication::~NativeApplication() noexcept = default;

	NativeWindow NativeApplication::create_window(NativeWindowCallbacks& callbacks)
	{
		NativeWindow result{ ::CreateWindowExW(WS_EX_APPWINDOW, WindowClass::Name, L"", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, _hinstance, this) };
		if (!result)
			throw_last_error("CreateWindowEx");
		_windows.emplace(result, &callbacks);
		return result;
	}

	bool NativeApplication::process_events()
	{
		// TODO: Process VK_SNAPSHOT, VK_{L,R}SHIFT, VK_{L,R}CONTROL, VK_{L,R}MENU.
		MSG msg;
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!::GetMessageW(&msg, NULL, 0, 0))
				return false;
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
		return true;
	}

	NativeWindowCallbacks* NativeApplication::find_callbacks(HWND hwnd) const noexcept
	{
		const auto i = _windows.find(hwnd);
		return i != _windows.end() ? i->second : nullptr;
	}

	LRESULT NativeApplication::window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto key_from_wparam = [](WPARAM vk) {
			if (vk >= '0' && vk <= '9')
			{
				return static_cast<Key>(to_underlying(Key::_0) + vk - '0');
			}
			else if (vk >= 'A' && vk <= 'Z')
			{
				return static_cast<Key>(to_underlying(Key::A) + vk - 'A');
			}
			else if (vk >= VK_F1 && vk <= VK_F24)
			{
				return static_cast<Key>(to_underlying(Key::F1) + vk - VK_F1);
			}
			else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
			{
				return static_cast<Key>(to_underlying(Key::Num0) + vk - VK_NUMPAD0);
			}
			else
				switch (vk)
				{
				case VK_LBUTTON: return Key::Mouse1;
				case VK_RBUTTON: return Key::Mouse2;
				case VK_MBUTTON: return Key::Mouse3;
				case VK_XBUTTON1: return Key::Mouse4;
				case VK_XBUTTON2: return Key::Mouse5;
				case VK_BACK: return Key::Backspace;
				case VK_TAB: return Key::Tab;
				case VK_RETURN: return Key::Enter;
				case VK_PAUSE: return Key::Pause;
				case VK_ESCAPE: return Key::Escape;
				case VK_SPACE: return Key::Space;
				case VK_PRIOR: return Key::PageUp;
				case VK_NEXT: return Key::PageDown;
				case VK_END: return Key::End;
				case VK_HOME: return Key::Home;
				case VK_LEFT: return Key::Left;
				case VK_UP: return Key::Up;
				case VK_RIGHT: return Key::Right;
				case VK_DOWN: return Key::Down;
				case VK_SNAPSHOT: return Key::Print;
				case VK_INSERT: return Key::Insert;
				case VK_DELETE: return Key::Delete;
				case VK_MULTIPLY: return Key::Multiply;
				case VK_ADD: return Key::Add;
				case VK_SUBTRACT: return Key::Subtract;
				case VK_DECIMAL: return Key::Decimal;
				case VK_DIVIDE: return Key::Divide;
				case VK_LSHIFT: return Key::LShift;
				case VK_RSHIFT: return Key::RShift;
				case VK_LCONTROL: return Key::LControl;
				case VK_RCONTROL: return Key::RControl;
				case VK_LMENU: return Key::LAlt;
				case VK_RMENU: return Key::RAlt;
				case VK_OEM_1: return Key::Semicolon;
				case VK_OEM_PLUS: return Key::Equals;
				case VK_OEM_COMMA: return Key::Comma;
				case VK_OEM_MINUS: return Key::Minus;
				case VK_OEM_PERIOD: return Key::Period;
				case VK_OEM_2: return Key::Slash;
				case VK_OEM_3: return Key::Grave;
				case VK_OEM_4: return Key::LBracket;
				case VK_OEM_5: return Key::Backslash;
				case VK_OEM_6: return Key::RBracket;
				case VK_OEM_7: return Key::Apostrophe;
				}
			return Key::None;
		};

		const auto get_modifiers = [] {
			Flags<KeyEvent::Modifier> modifiers;
			if (::GetKeyState(VK_SHIFT) & 0x8000)
				modifiers |= KeyEvent::Modifier::Shift;
			if (::GetKeyState(VK_CONTROL) & 0x8000)
				modifiers |= KeyEvent::Modifier::Control;
			if (::GetKeyState(VK_MENU) & 0x8000)
				modifiers |= KeyEvent::Modifier::Alt;
			return modifiers;
		};

		const auto call_mouse_key = [this, hwnd, &get_modifiers](Key key, bool pressed) {
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_key(key, pressed, false, get_modifiers());
		};

		switch (msg)
		{
		case WM_CLOSE:
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_close();
			break;

		case WM_DESTROY:
			// cppcheck-suppress stlIfFind
			if (const auto i = _windows.find(hwnd); i != _windows.end())
			{
				_windows.erase(i);
				if (_windows.empty())
					::PostQuitMessage(0);
			}
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_key(key_from_wparam(wparam), true, lparam & 0x40000000, get_modifiers());
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_key(key_from_wparam(wparam), false, false, get_modifiers());
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			call_mouse_key(Key::Mouse1, msg == WM_LBUTTONDOWN);
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			call_mouse_key(Key::Mouse2, msg == WM_RBUTTONDOWN);
			break;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			call_mouse_key(Key::Mouse3, msg == WM_MBUTTONDOWN);
			break;

		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			call_mouse_key(GET_XBUTTON_WPARAM(wparam) == XBUTTON1 ? Key::Mouse4 : Key::Mouse5, msg == WM_XBUTTONDOWN);
			return TRUE;

		case WM_ACTIVATE:
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_focus(wparam == WA_ACTIVE || wparam == WA_CLICKACTIVE);
			break;

		case WM_SIZE:
			if (const auto callbacks = find_callbacks(hwnd))
				callbacks->on_resize({ LOWORD(lparam), HIWORD(lparam) });
			break;

		case WM_UNICHAR:
			if (wparam == UNICODE_NOCHAR)
				return 1;
			[[fallthrough]];
		case WM_CHAR:
			if (const auto callbacks = find_callbacks(hwnd))
			{
				std::array<char, 4> buffer;
				if (const auto bytes = primal::writeUtf8(buffer, static_cast<char32_t>(wparam)))
					if (const auto c = static_cast<unsigned char>(buffer[0]); c >= 0x20 && c != 0x7f)
						callbacks->on_text({ buffer.data(), bytes });
			}
			break;

		default:
			return ::DefWindowProcW(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

	LRESULT NativeApplication::static_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		NativeApplication* application = nullptr;
		if (msg == WM_NCCREATE)
		{
			application = static_cast<NativeApplication*>(((CREATESTRUCTW*)lparam)->lpCreateParams);
			::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)application);
		}
		else
			application = (NativeApplication*)::GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		return application->window_proc(hwnd, msg, wparam, lparam);
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
			log_last_error("DestroyCursor");
	}

	NativeApplication::WindowClass::WindowClass(HINSTANCE hinstance, HCURSOR hcursor, WNDPROC wndproc)
		: _hinstance{ hinstance }
	{
		WNDCLASSEXW wndclass{ sizeof wndclass };
		wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		wndclass.lpfnWndProc = wndproc;
		wndclass.hInstance = _hinstance;
		wndclass.hIcon = ::LoadIconA(nullptr, IDI_APPLICATION);
		wndclass.hCursor = hcursor;
		wndclass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wndclass.lpszClassName = Name;
		if (!::RegisterClassExW(&wndclass))
			throw_last_error("RegisterClassEx");
	}

	NativeApplication::WindowClass::~WindowClass()
	{
		if (!::UnregisterClassW(Name, _hinstance))
			log_last_error("UnregisterClass");
	}
}
