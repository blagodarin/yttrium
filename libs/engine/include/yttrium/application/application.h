// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/api.h>

#include <chrono>
#include <functional>
#include <memory>

namespace Yt
{
	///
	class Y_ENGINE_API Application
	{
	public:
		Application();
		~Application() noexcept;

		///
		void run(const std::function<void(const std::chrono::milliseconds&)>&);

	private:
		const std::unique_ptr<class ApplicationPrivate> _private;
		friend ApplicationPrivate;
	};
}
