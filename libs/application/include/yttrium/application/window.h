// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include <memory>
#include <string_view>

namespace Yt
{
	class Application;
	class Image;
	class KeyEvent;
	class Point;
	class Size;
	struct WindowID;

	///
	class Window
	{
	public:
		///
		explicit Window(Application&, const std::string& title = {});

		///
		~Window();

		///
		void close();

		///
		Point cursor() const;

		///
		WindowID id() const noexcept;

		///
		void lock_cursor(bool lock);

		///
		void on_cursor_moved(const std::function<void(int dx, int dy)>&);

		///
		void on_key_event(const std::function<void(const KeyEvent&)>&);

		///
		void on_text_input(const std::function<void(std::string_view)>&);

		///
		bool set_cursor(const Point& cursor);

		///
		void set_icon(const Image&);

		///
		void set_title(const std::string&);

		///
		void show();

		///
		Size size() const;

		///
		void swap_buffers();

	private:
		const std::unique_ptr<class WindowPrivate> _private;
	};
}
