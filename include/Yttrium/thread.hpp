/// \file
/// \brief %Thread facilities.

#ifndef __Y_THREAD_HPP
#define __Y_THREAD_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/noncopyable.hpp>
#include <Yttrium/time.hpp>

namespace Yttrium
{

/// %Thread.

class Y_API Thread: public Noncopyable
{
public:

	///

	Thread(Allocator *allocator = HeapAllocator::instance()) noexcept;

	///

	virtual ~Thread() noexcept;

public:

	/// Return \c true if the thread is running, \c false otherwise.
	/// \return \c true for a running thread.

	bool is_running() noexcept;

	/// Start the thread execution.
	/// \note The thread must not be running.

	void start() noexcept;

	/// Force the thread to stop execution.

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

	friend class Private;

private:

	Allocator *_allocator;
	Private   *_private;
};

} // namespace Yttrium

#endif // __Y_THREAD_HPP
