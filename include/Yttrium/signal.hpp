/// \file
/// \brief Cross-thread signals.

#ifndef __Y_SIGNAL_HPP
#define __Y_SIGNAL_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/time.hpp>

namespace Yttrium
{

class Mutex;

///

class Y_API Signal
{
public:

	///

	Signal(Allocator *allocator = HeapAllocator::instance()) noexcept;

	///

	~Signal();

public:

	///

	void wait(Mutex *mutex) noexcept;

	///

	bool try_wait(Mutex *mutex) noexcept;

	///

	bool try_wait(Mutex *mutex, Clock milliseconds) noexcept;

	///

	void signal();

public:

	class Private;

protected:

	Signal(Private *private_)
		: _private(private_)
	{
	}

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SIGNAL_HPP
