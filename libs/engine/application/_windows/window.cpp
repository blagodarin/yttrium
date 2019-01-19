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

#include "window.h"

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/key.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include "../../../core/image/formats/bmp.h"
#include "../window_callbacks.h"
#include "error.h"

#include <cassert>

namespace Yttrium
{
	WindowBackend::WindowDC::WindowDC(HWND hwnd)
		: _hdc{ ::GetDC(hwnd) } // Private DCs don't need to be released.
	{
		if (!_hdc)
			throw InitializationError{ "GetDC failed" };
	}

	WindowBackend::WindowBackend(WindowBackendCallbacks& callbacks)
		: _callbacks{ callbacks }
	{
		RECT client_rect{ 0, 0, 0, 0 };
		::GetClientRect(_hwnd, &client_rect);
		_size = { client_rect.right - client_rect.left, client_rect.bottom - client_rect.top };
	}

	void WindowBackend::close()
	{
		::SendMessageW(_hwnd, WM_CLOSE, 0, 0);
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		POINT gdi_cursor;
		if (!::GetCursorPos(&gdi_cursor)
			|| !::ScreenToClient(_hwnd, &gdi_cursor))
			return false;
		cursor = { gdi_cursor.x, gdi_cursor.y };
		return true;
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		POINT gdi_cursor{ cursor._x, cursor._y };
		return ::ClientToScreen(_hwnd, &gdi_cursor)
			&& ::SetCursorPos(gdi_cursor.x, gdi_cursor.y);
	}

	void WindowBackend::set_icon(const Image& icon)
	{
		const auto width = icon.format().width();
		const auto height = icon.format().height();
		const auto image_size = width * height * sizeof(std::uint32_t);
		const auto mask_size = (width + 7) / 8 * height;
		const auto buffer_size = sizeof(BmpInfoHeader) + image_size + mask_size;
		const auto buffer = std::make_unique<std::uint8_t[]>(buffer_size);
		auto* header = reinterpret_cast<BmpInfoHeader*>(buffer.get());
		header->header_size = sizeof *header;
		header->width = static_cast<std::int32_t>(width);
		header->height = static_cast<std::int32_t>(height) * 2;
		header->planes = 1;
		header->bits_per_pixel = 32;
		header->compression = BmpCompression::Rgb;
		header->image_size = static_cast<std::uint32_t>(image_size);
		header->x_pixels_per_meter = 0;
		header->y_pixels_per_meter = 0;
		header->used_colors = 0;
		header->required_colors = 0;
		auto bgra_icon = to_bgra(icon); // TODO: Use buffer data as image conversion output.
		bgra_icon.flip_vertically();    // TODO: Check image orientation.
		std::memcpy(buffer.get() + sizeof *header, bgra_icon.data(), image_size);
		std::memset(buffer.get() + sizeof *header + image_size, 0xff, mask_size);
		const auto hicon = ::CreateIconFromResourceEx(buffer.get(), static_cast<DWORD>(buffer_size), TRUE, 0x00030000, 0, 0, LR_DEFAULTCOLOR);
		::SendMessageW(_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hicon));
		::SendMessageW(_hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hicon));
		// TODO: Free icon handle.
	}

	void WindowBackend::set_title(const std::string& title)
	{
		const auto title_size = ::MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
		if (!title_size)
			throw_last_error("MultiByteToWideChar");
		const auto title_buffer = std::make_unique<wchar_t[]>(title_size);
		if (!::MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, title_buffer.get(), title_size))
			throw_last_error("MultiByteToWideChar");
		if (!::SetWindowTextW(_hwnd, title_buffer.get()))
			throw_last_error("SetWindowText");
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

	void WindowBackend::on_close()
	{
		_hwnd.reset();
	}

	void WindowBackend::on_focus(bool focused)
	{
		_callbacks.on_focus_event(focused);
	}

	void WindowBackend::on_key(Key key, bool pressed, Flags<KeyEvent::Modifier> modifiers)
	{
		_callbacks.on_key_event(key, pressed, modifiers);
	}

	void WindowBackend::on_resize(const Size& size)
	{
		_size = size;
		_callbacks.on_resize_event(_size);
	}

	void WindowBackend::on_text(std::string_view text)
	{
		_callbacks.on_text_input(text);
	}
}
