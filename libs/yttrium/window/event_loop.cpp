#include <yttrium/event_loop.h>
#include "event_loop.h"

#include "window.h"

#include <stdexcept>

namespace Yttrium
{
	void EventLoopPrivate::add_window(EventLoop& event_loop, WindowPrivate& window)
	{
		WindowPrivate* expected = nullptr;
		if (!event_loop._private->_window.compare_exchange_strong(expected, &window))
			throw std::logic_error{ "An EventLoop can't have multiple Windows" };
	}

	EventLoop::EventLoop()
		: _private{ std::make_unique<EventLoopPrivate>() }
	{
	}

	EventLoop::~EventLoop() noexcept = default;

	void EventLoop::run()
	{
		if (auto* window = _private->window())
			window->run();
	}
}
