// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>

namespace Yt
{
	class Application;
	class EventCallbacks;
	class WindowPrivate;

	class ApplicationPrivate
	{
	public:
		bool process_events(EventCallbacks&);
		WindowPrivate* window() const noexcept { return _window.load(); }

		static void add_window(Application&, WindowPrivate&);

	private:
		std::atomic<WindowPrivate*> _window{ nullptr };

		friend Application;
	};

	struct ApplicationStub
	{
	public:
		ApplicationStub(Application& application, WindowPrivate& window)
		{
			ApplicationPrivate::add_window(application, window);
		}
	};
}
