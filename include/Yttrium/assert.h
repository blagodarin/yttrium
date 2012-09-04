/// \file
/// \brief Run-time assertions.

#ifndef __Y_ASSERT_H
#define __Y_ASSERT_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

/// Terminate the program, outputting a message to the log.
/// \param message The message to output.

Y_API void abort(const StaticString &message, ...) noexcept;

} // namespace Yttrium

/// Terminate the program, outputting a message to the log.

#define Y_ABORT(...) Yttrium::abort(__VA_ARGS__)

/// Terminate the program if \a condition is true, outputting a message to the log.

#define Y_ABORT_IF(condition, ...) \
	do \
	{ \
		if (Y_UNLIKELY(condition)) \
		{ \
			Yttrium::abort(__VA_ARGS__); \
		} \
	} while (false)

/// \def Y_ASSERT(condition)
/// \brief Terminate the program if the \a condition isn't met.

#if !Y_IS_DEBUG
	#define Y_ASSERT(condition)
#else
	#define Y_ASSERT(condition) \
		do \
		{ \
			if (Y_UNLIKELY(!(condition))) \
			{ \
				Y_ABORT(S("Assertion failed: " #condition)); \
			} \
		} while (false)
#endif

#endif // __Y_ASSERT_H
