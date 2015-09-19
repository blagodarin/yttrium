/// \file
/// \brief Run-time assertions.

#ifndef _include_yttrium_assert_h_
#define _include_yttrium_assert_h_

#include <yttrium/global.h>

namespace Yttrium
{
	/// Terminate the program, outputting a message to the log.
	/// \param message Message to output.
	/// \param file Source file.
	/// \param line Source line.
	/// \param function Source function.
	Y_API Y_NORETURN void abort(const char* message, const char* file, int line);
}

/// Terminate the program, outputting a message to the log.
#define Y_ABORT(message) \
	Yttrium::abort(message, __FILE__, __LINE__)

/// Terminate the program if \a condition is true, outputting a message to the log.
#define Y_ABORT_IF(condition, message) \
	do { if (condition) Y_ABORT(message); } while (false)

/// \def Y_ASSERT(condition)
/// \brief Terminate the program if the \a condition isn't met.
#if !Y_IS_DEBUG
	#define Y_ASSERT(condition)
#else
	#define Y_ASSERT(condition) \
		do { if (!(condition)) Y_ABORT("Assertion failed: " #condition); } while (false)
#endif

/// Terminate the program if the values differ.
#define Y_VERIFY(expected, actual) \
	do { if ((expected) != (actual)) Y_ABORT(#expected" != "#actual); } while (false)

#endif
