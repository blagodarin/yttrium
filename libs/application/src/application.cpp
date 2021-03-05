// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/application.h>
#include "application.h"

#include "window.h"

#include <stdexcept>

namespace Yt
{
	bool ApplicationPrivate::process_events()
	{
		if (auto* window = _window.load(); window && window->process_events())
		{
			window->update();
			return true;
		}
		return false;
	}

	void ApplicationPrivate::add_window(Application& application, WindowPrivate& window)
	{
		WindowPrivate* expected = nullptr;
		if (!application._private->_window.compare_exchange_strong(expected, &window))
			throw std::logic_error{ "An Application can't have multiple Windows" };
	}

	Application::Application()
		: _private{ std::make_unique<ApplicationPrivate>() }
	{
	}

	Application::~Application() noexcept = default;

	bool Application::process_events()
	{
		return _private->process_events();
	}
}
