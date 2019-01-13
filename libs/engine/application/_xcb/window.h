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

#pragma once

#include <yttrium/math/size.h>
#include "application.h"

#include <optional>

namespace Yttrium
{
	class Image;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:
		WindowBackend(WindowBackendCallbacks&);
		~WindowBackend() noexcept;

		void close() noexcept;
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void set_icon(const Image&);
		void set_title(const std::string&);
		void show();
		std::optional<Size> size() const noexcept { return _size; }
		void swap_buffers();

		xcb_connection_t* xcb_connection() const noexcept { return _application.connection(); }
		xcb_window_t xcb_window() const noexcept { return _window; }

	private:
		using P_Atom = UniquePtr<xcb_intern_atom_reply_t, std::free>;

		P_Atom make_atom(std::string_view);

	private:
		class EmptyCursor;
		class Keyboard;

		WindowBackendCallbacks& _callbacks;
		NativeApplication _application;
		P_Atom _wm_protocols{ make_atom("WM_PROTOCOLS") };
		P_Atom _wm_delete_window{ make_atom("WM_DELETE_WINDOW") };
		std::unique_ptr<Keyboard> _keyboard;
		xcb_window_t _window = XCB_WINDOW_NONE;
		std::unique_ptr<EmptyCursor> _empty_cursor;
		std::optional<Size> _size;
	};
}
