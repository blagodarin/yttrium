//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <atomic>
#include <chrono>
#include <functional>

namespace Yt
{
	class Application;
	class WindowPrivate;

	class ApplicationPrivate
	{
	public:
		bool process_events();
		WindowPrivate* window() const noexcept { return _window.load(); }

		static void add_window(Application&, WindowPrivate&);

	private:
		std::function<void(std::chrono::milliseconds)> _on_update;
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
