/// \file
/// \brief %Mutex facilities.

#ifndef __Y_MUTEX_H
#define __Y_MUTEX_H

#include <Yttrium/time.h>

namespace Yttrium
{

class Signal;

/// %Mutex locker.

template <typename T>
class MutexLocker
{
public:

	///

	MutexLocker(T *mutex) noexcept
		: _mutex(mutex)
	{
		_mutex->lock();
	}

	///

	~MutexLocker() noexcept
	{
		_mutex->unlock();
	}

private:

	T *_mutex;
};

/// %Thread mutex.

class Y_API Mutex
{
	friend class Signal;

public:

	/// %Lock type.

	typedef MutexLocker<Mutex> Locker;

public:

	///

	Mutex(Allocator *allocator = DefaultAllocator) noexcept;

	///

	Mutex(const Mutex &mutex) noexcept;

	///

	~Mutex() noexcept
	{
		close();
	}

public:

	/// Lock the mutex for the current thread.
	/// \note If the mutex is locked, the calling thread will be suspended
	/// until the mutex is released.

	void lock() noexcept;

	/// Try to lock the mutex.
	/// \return \c true if the mutex was locked successfully.

	bool try_lock() noexcept;

	///

	bool try_lock(Clock milliseconds) noexcept;

	/// Unlock the mutex.
	/// \note The mutex must be unlocked to be destroyed properly.

	void unlock() noexcept;

public:

	///

	Mutex &operator =(const Mutex &mutex) noexcept;

public:

	class Private;

protected:

	Mutex(Private *private_)
		: _private(private_)
	{
	}

private:

	void close() noexcept;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_MUTEX_H
