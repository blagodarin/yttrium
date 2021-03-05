// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/api.h>

#include <functional>
#include <memory>
#include <string_view>

namespace Yt
{
	class Application;
	struct RenderReport;
	class Image;
	class KeyEvent;
	class Point;
	class RenderManager;
	class RenderPass;
	class Size;
	class Vector2;

	///
	class Y_ENGINE_API Window
	{
	public:
		///
		explicit Window(Application&);

		///
		~Window();

		///
		void close();

		///
		Point cursor() const;

		///
		void lock_cursor(bool lock);

		///
		void on_cursor_moved(const std::function<void(int dx, int dy)>&);

		///
		void on_key_event(const std::function<void(const KeyEvent&)>&);

		///
		void on_render(const std::function<void(RenderPass&, const Vector2& cursor, const RenderReport&)>&);

		/// Sets a callback to be called when a screenshot image is ready.
		void on_screenshot(const std::function<void(Image&&)>&);

		///
		void on_text_input(const std::function<void(std::string_view)>&);

		///
		RenderManager& render_manager();

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

		/// Requests a screenshot to be taken.
		void take_screenshot();

	private:
		const std::unique_ptr<class WindowPrivate> _private;
	};
}
