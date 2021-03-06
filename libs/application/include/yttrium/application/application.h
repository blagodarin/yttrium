// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class EventCallbacks;

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
