/// \file
/// \brief %Thread facilities.

#ifndef __Y_THREAD_H
#define __Y_THREAD_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/time.h>

namespace Yttrium
{

/// %Thread.

class Y_API Thread: public Noncopyable
{
public:

	///

	Thread(Allocator *allocator = DefaultAllocator) noexcept;

	///

	virtual ~Thread() noexcept;

public:

	/// Return \c true if the thread is running, \c false otherwise.
	/// \return \c true for a running thread.

	bool is_running() noexcept;

	/// Start the thread.

	void start() noexcept;

	/// Force the thread to stop.

	void stop() noexcept;

	/// Wait for the thread to terminate.

	void wait() noexcept;

protected:

	///

	virtual void run() = 0;

public:

	/// Suspend the execution of the current thread for a given amount of time.
	/// \param milliseconds Time to suspend the execution for.
	/// \note Passing zero \a milliseconds will cause the suspension to last for a
	/// minimal still nonzero amount of time possible. Therefore, it is
	/// recommended to call <tt>Thread::sleep(0)</tt> in heavy and long-lasting
	/// loops.

	static void sleep(Clock milliseconds = 0) noexcept;

private:

	class Private;

	friend Private;

	Allocator *_allocator; // NOTE: Why store an allocator? Private should already have one.
	Private   *_private;
};

} // namespace Yttrium

#endif // __Y_THREAD_H
