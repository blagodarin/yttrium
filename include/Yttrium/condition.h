/// \file
/// \brief

#ifndef __Y_CONDITION_H
#define __Y_CONDITION_H

#include <Yttrium/time.h>

namespace Yttrium
{

class Mutex;

///

class Y_API Condition
{
public:

	///

	Condition(Allocator *allocator = DefaultAllocator) noexcept;

	///

	Condition(const Condition &condition) noexcept;

	///

	~Condition() noexcept
	{
		close();
	}

public:

	///

	void notify_all() noexcept;

	///

	void notify_one() noexcept;

	///

	bool try_wait(Mutex *mutex) noexcept;

	///

	bool try_wait(Mutex *mutex, Clock milliseconds = 0) noexcept;

	///

	void wait(Mutex *mutex) noexcept;

public:

	///

	Condition &operator =(const Condition &condition) noexcept;

public:

	class Private;

protected:

	Condition(Private *private_)
		: _private(private_)
	{
	}

private:

	void close() noexcept;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_CONDITION_H
