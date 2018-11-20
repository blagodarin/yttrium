//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/application.h>
#include "application.h"

#include "window.h"

#include <stdexcept>

namespace Yttrium
{
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

	void Application::run()
	{
		if (auto* window = _private->window())
			window->run();
	}
}
