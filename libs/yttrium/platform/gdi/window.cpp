#include "window.h"

#include <yttrium/exceptions.h>
#include <yttrium/key.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include "../../window/backend.h"

#include <cassert>

namespace
{
	using namespace Yttrium;

	Key key_from_wparam(WPARAM vk)
	{
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
		else switch (vk)
		{
		case VK_LBUTTON:    return Key::Mouse1;
		case VK_RBUTTON:    return Key::Mouse2;
		case VK_MBUTTON:    return Key::Mouse3;
		case VK_XBUTTON1:   return Key::Mouse4;
		case VK_XBUTTON2:   return Key::Mouse5;
		case VK_BACK:       return Key::Backspace;
		case VK_TAB:        return Key::Tab;
		case VK_RETURN:     return Key::Enter;
		case VK_PAUSE:      return Key::Pause;
		case VK_ESCAPE:     return Key::Escape;
		case VK_SPACE:      return Key::Space;
		case VK_PRIOR:      return Key::PageUp;
		case VK_NEXT:       return Key::PageDown;
		case VK_END:        return Key::End;
		case VK_HOME:       return Key::Home;
		case VK_LEFT:       return Key::Left;
		case VK_UP:         return Key::Up;
		case VK_RIGHT:      return Key::Right;
		case VK_DOWN:       return Key::Down;
		case VK_SNAPSHOT:   return Key::Print;
		case VK_INSERT:     return Key::Insert;
		case VK_DELETE:     return Key::Delete;
		case VK_MULTIPLY:   return Key::Multiply;
		case VK_ADD:        return Key::Add;
		case VK_SUBTRACT:   return Key::Subtract;
		case VK_DECIMAL:    return Key::Decimal;
		case VK_DIVIDE:     return Key::Divide;
		case VK_LSHIFT:     return Key::LShift;
		case VK_RSHIFT:     return Key::RShift;
		case VK_LCONTROL:   return Key::LControl;
		case VK_RCONTROL:   return Key::RControl;
		case VK_LMENU:      return Key::LAlt;
		case VK_RMENU:      return Key::RAlt;
		case VK_OEM_1:      return Key::Semicolon;
		case VK_OEM_PLUS:   return Key::Equals;
		case VK_OEM_COMMA:  return Key::Comma;
		case VK_OEM_MINUS:  return Key::Minus;
		case VK_OEM_PERIOD: return Key::Period;
		case VK_OEM_2:      return Key::Slash;
		case VK_OEM_3:      return Key::Grave;
		case VK_OEM_4:      return Key::LBracket;
		case VK_OEM_5:      return Key::Backslash;
		case VK_OEM_6:      return Key::RBracket;
		case VK_OEM_7:      return Key::Apostrophe;
		}
		return Key::Null;
	}
}

namespace Yttrium
{
	WindowBackend::WindowClass::WindowClass(HINSTANCE hinstance, WNDPROC wndproc)
		: _hinstance{hinstance}
	{
		_wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		_wndclass.lpfnWndProc = wndproc;
		_wndclass.hInstance = _hinstance;
		_wndclass.hIcon = ::LoadIconA(nullptr, IDI_APPLICATION);
		_wndclass.hCursor = ::LoadCursorA(nullptr, IDC_ARROW); // TODO: Empty cursor.
		_wndclass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		_wndclass.lpszClassName = "Yttrium";
		if (!::RegisterClassExA(&_wndclass))
			throw InitializationError{"Failed to register window class"};
	}

	WindowBackend::WindowClass::~WindowClass()
	{
		::UnregisterClass(_wndclass.lpszClassName, _hinstance);
	}

	WindowBackend::WindowHandle::WindowHandle(const WindowClass& window_class, const char* title, void* user_data)
		: _hwnd{::CreateWindowExA(WS_EX_APPWINDOW, window_class.name(), title, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, window_class.hinstance(), user_data)}
	{
		if (!_hwnd)
			throw InitializationError{"Failed to create window"};
	}

	WindowBackend::WindowHandle::~WindowHandle()
	{
		::DestroyWindow(_hwnd);
	}

	WindowBackend::WindowDC::WindowDC(const WindowHandle& hwnd)
		: _hwnd{hwnd}
		, _hdc{::GetDC(_hwnd)}
	{
		if (!_hdc)
			throw InitializationError{"Failed to get device context"};
	}

	WindowBackend::WindowDC::~WindowDC()
	{
		::ReleaseDC(_hwnd, _hdc);
	}

	WindowBackend::WindowBackend(const std::string& name, WindowBackendCallbacks& callbacks)
		: _name{name}
		, _callbacks{callbacks}
	{
		assert(_size); // Window size is initialized during member construction (from window procedure).
		_created = true;
	}

	void WindowBackend::close()
	{
		::SendMessageA(_hwnd, WM_CLOSE, 0, 0);
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		POINT gdi_cursor;
		if (!::GetCursorPos(&gdi_cursor))
			return false;
		::ScreenToClient(_hwnd, &gdi_cursor);
		cursor = {gdi_cursor.x, gdi_cursor.y};
		return true;
	}

	bool WindowBackend::process_events()
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

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		POINT gdi_cursor{cursor._x, cursor._y};
		::ClientToScreen(_hwnd, &gdi_cursor);
		return ::SetCursorPos(gdi_cursor.x, gdi_cursor.y);
	}

	void WindowBackend::show()
	{
		::ShowWindow(_hwnd, SW_MAXIMIZE);
		::UpdateWindow(_hwnd);
		::SetForegroundWindow(_hwnd);
		::SetFocus(_hwnd);
	}

	void WindowBackend::swap_buffers()
	{
		::SwapBuffers(_hdc);
	}

	LRESULT WindowBackend::window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			::DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			assert(_created);
			_callbacks.on_key_event(::key_from_wparam(wparam), true, {});
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			assert(_created);
			_callbacks.on_key_event(::key_from_wparam(wparam), false, {});
			break;

		case WM_MOUSEWHEEL:
			{
				const auto wheel = GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
				(void)wheel; // TODO: Handle mouse wheel.
			}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			assert(_created);
			{
				const auto buttons = GET_KEYSTATE_WPARAM(wparam);
				_callbacks.on_key_event(Key::Mouse1, buttons & MK_LBUTTON, {});
				_callbacks.on_key_event(Key::Mouse2, buttons & MK_RBUTTON, {});
				_callbacks.on_key_event(Key::Mouse3, buttons & MK_MBUTTON, {});
				_callbacks.on_key_event(Key::Mouse4, buttons & MK_XBUTTON1, {});
				_callbacks.on_key_event(Key::Mouse5, buttons & MK_XBUTTON2, {});
			}
			break;

		case WM_ACTIVATE:
			assert(_created);
			_callbacks.on_focus_event(wparam == WA_ACTIVE || wparam == WA_CLICKACTIVE);
			break;

		case WM_SIZE:
			_size.emplace(LOWORD(lparam), HIWORD(lparam));
			if (_created)
				_callbacks.on_resize_event(*_size);
			break;

		default:
			return ::DefWindowProcA(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

	LRESULT WindowBackend::static_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		WindowBackend* window = nullptr;
		if (msg == WM_NCCREATE)
		{
			window = static_cast<WindowBackend*>(((CREATESTRUCTA*)lparam)->lpCreateParams);
			::SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		}
		else
			window = (WindowBackend*)::GetWindowLongPtrA(hwnd, GWLP_USERDATA);
		return window->window_proc(hwnd, msg, wparam, lparam);
	}
}
