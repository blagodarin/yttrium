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
