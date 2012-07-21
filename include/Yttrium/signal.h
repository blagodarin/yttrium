/// \file
/// \brief Cross-thread signals.

#ifndef __Y_SIGNAL_H
#define __Y_SIGNAL_H

#include <Yttrium/allocator.h>
#include <Yttrium/time.h>

namespace Yttrium
{

class Mutex;

///

class Y_API Signal
{
public:

	///

	Signal(Allocator *allocator = DefaultAllocator) noexcept;

	///

	Signal(const Signal &signal) noexcept;

	///

	~Signal() noexcept
	{
		close();
	}

public:

	///

	void wait(Mutex *mutex) noexcept;

	///

	bool try_wait(Mutex *mutex) noexcept;

	///

	bool try_wait(Mutex *mutex, Clock milliseconds) noexcept;

	///

	void signal() noexcept;

public:

	///

	Signal &operator =(const Signal &signal) noexcept;

public:

	class Private;

protected:

	Signal(Private *private_)
		: _private(private_)
	{
	}

private:

	void close() noexcept;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SIGNAL_H
