// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/window.h>
#include <yttrium/geometry/size.h>
#include "application.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

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
		WindowID id() const noexcept { return { _application.connection(), static_cast<intptr_t>(_window) }; }
		bool process_events();
		bool set_cursor(const Point&);
		void set_icon(const Image&);
		void set_title(const std::string&);
		void show();
		std::optional<Size> size() const noexcept { return _size; }
		void swap_buffers();

	private:
		using P_Atom = seir::CPtr<xcb_intern_atom_reply_t, ::free>;

		P_Atom make_atom(std::string_view);

	private:
		class EmptyCursor;
		class Keyboard;

		using P_Event = seir::CPtr<xcb_generic_event_t, ::free>;

		WindowBackendCallbacks& _callbacks;
		NativeApplication _application;
		P_Atom _wm_protocols{ make_atom("WM_PROTOCOLS") };
		P_Atom _wm_delete_window{ make_atom("WM_DELETE_WINDOW") };
		std::unique_ptr<Keyboard> _keyboard;
		xcb_window_t _window = XCB_WINDOW_NONE;
		std::unique_ptr<EmptyCursor> _empty_cursor;
		std::optional<Size> _size;
		std::vector<P_Event> _events;
	};
}
