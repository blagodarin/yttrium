// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

namespace seir
{
	class Image;
	class Point;
	class Size;
}

namespace Yt
{
	class Application;

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
		seir::Point cursor() const;

		///
		WindowID id() const noexcept;

		///
		void lock_cursor(bool lock);

		///
		bool set_cursor(const seir::Point& cursor);

		///
		void set_icon(const seir::Image&);

		///
		void set_title(const std::string&);

		///
		void show();

		///
		seir::Size size() const;

		///
		void swap_buffers();

	private:
		const std::unique_ptr<class WindowPrivate> _private;
	};
}
