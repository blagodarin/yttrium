// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>

namespace Yt
{
	class KeyEvent;
	class Window;

	class EventCallbacks
	{
	public:
		virtual ~EventCallbacks() noexcept = default;
		virtual void onWindowMouseMove(const Window&, int dx, int dy) = 0;
		virtual void onWindowKeyEvent(const Window&, const KeyEvent&) = 0;
		virtual void onWindowTextInput(const Window&, std::string_view) = 0;
	};

	///
	class Application
	{
	public:
		Application();
		~Application() noexcept;

		///
		bool process_events(EventCallbacks&);

	private:
		const std::unique_ptr<class ApplicationPrivate> _private;
		friend ApplicationPrivate;
	};
}
