#pragma once

#include <atomic>

namespace Yttrium
{
	class EventLoop;
	class WindowPrivate;

	class EventLoopPrivate
	{
	public:
		WindowPrivate* window() const noexcept { return _window.load(); }

		static void add_window(EventLoop&, WindowPrivate&);

	private:
		std::atomic<WindowPrivate*> _window{ nullptr };
	};

	struct EventLoopStub
	{
	public:
		EventLoopStub(EventLoop& event_loop, WindowPrivate& window)
		{
			EventLoopPrivate::add_window(event_loop, window);
		}
	};
}
