#ifndef _include_yttrium_event_loop_h_
#define _include_yttrium_event_loop_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Y_API EventLoop
	{
	public:
		EventLoop();
		~EventLoop() noexcept;

		void run();

	private:
		const std::unique_ptr<class EventLoopPrivate> _private;
		friend EventLoopPrivate;
	};
}

#endif
