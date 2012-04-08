/// \file
/// \brief %Thread facilities.

#ifndef __Y_THREAD_HPP
#define __Y_THREAD_HPP

#include <functional> // std::function

#include <Yttrium/allocator.hpp>
#include <Yttrium/time.hpp>

namespace Yttrium
{

/// %Thread.

class Y_API Thread
{
public:

	///

	typedef std::function<void()> Function;

public:

	///

	Thread(Allocator *allocator = HeapAllocator::instance()) noexcept;

	///

	Thread(const Thread &thread) noexcept;

	///

	~Thread() noexcept;

public:

	/// Return \c true if the thread is running, \c false otherwise.
	/// \return \c true for a running thread.

	bool is_running() noexcept;

	/// Start the execution of the thread for a given function.
	/// \tparam function Function to execute.
	/// \note If the function fails while another function is executing for the
	/// thread object, its execution would not be stopped.
	/// \note This function is not reentrant.

	void start(const Function &function) noexcept;

	/// Force the thread to stop execution.

	void stop() noexcept;

	/// Wait for the thread to terminate.

	void wait() noexcept;

public:

	/// Suspend the execution of the current thread for a given amount of time.
	/// \param milliseconds Time to suspend the execution for.
	/// \note Passing zero \a milliseconds will cause the suspension to last for a
	/// minimal still nonzero amount of time possible. Therefore, it is
	/// recommended to call \c Thread::sleep(0) in heavy and long-lasting
	/// loops.

	static void sleep(Clock ms) noexcept;

public:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_THREAD_HPP
