#pragma once

#include <atomic>

namespace Yttrium
{
	class Application;
	class WindowPrivate;

	class ApplicationPrivate
	{
	public:
		WindowPrivate* window() const noexcept { return _window.load(); }

		static void add_window(Application&, WindowPrivate&);

	private:
		std::atomic<WindowPrivate*> _window{ nullptr };
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
