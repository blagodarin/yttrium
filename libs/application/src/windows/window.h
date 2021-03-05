// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/id.h>
#include <yttrium/math/size.h>
#include "application.h"

#if Y_RENDERER_OPENGL
#	include "wgl.h"
#endif

#include <optional>
#include <string>

namespace Yt
{
	class Image;
	class WindowBackendCallbacks;

	class WindowBackend : private NativeWindowCallbacks
	{
	public:
		explicit WindowBackend(WindowBackendCallbacks&);

		void close();
		bool get_cursor(Point&);
		WindowID id() const noexcept { return { reinterpret_cast<intptr_t>(_application.instance()), reinterpret_cast<intptr_t>(_hwnd.get()) }; }
		bool process_events() { return _application.process_events(); }
		bool set_cursor(const Point&);
		void set_icon(const Image&);
		void set_title(const std::string&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		void on_close() override;
		void on_focus(bool) override;
		void on_key(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) override;
		void on_resize(const Size&) override;
		void on_text(std::string_view) override;

	private:
		class WindowDC
		{
		public:
			explicit WindowDC(HWND);
			operator HDC() const noexcept { return _hdc; }

		private:
			const HDC _hdc;
		};

		struct HIconDeleter
		{
			static void free(HICON) noexcept;
		};

		WindowBackendCallbacks& _callbacks;
		NativeApplication _application;
		primal::Pointer<std::remove_pointer_t<HICON>, HIconDeleter> _icon;
		NativeWindow _hwnd = _application.create_window(*this);
		const WindowDC _hdc{ _hwnd };
#if Y_RENDERER_OPENGL
		const WglContext _wgl{ _hdc };
#endif
		Size _size;
	};
}
