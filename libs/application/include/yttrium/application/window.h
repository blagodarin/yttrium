// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

namespace seir
{
	class Image;
}

namespace Yt
{
	class Application;
	class Point;
	class Size;

	struct WindowID
	{
		void* _application = nullptr;
		intptr_t _window = 0;
		constexpr WindowID(void* application, intptr_t window) noexcept
			: _application{ application }, _window{ window } {}
	};

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
		bool set_cursor(const Point& cursor);

		///
		void set_icon(const seir::Image&);

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
