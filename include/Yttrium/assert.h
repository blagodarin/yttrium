/// \file
/// \brief Run-time assertions.

#ifndef __Y_ASSERT_H
#define __Y_ASSERT_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

/// Terminate the program, outputting a message to the log.
/// \param file Source file name (usually \c __FILE__).
/// \param line Source file line number (usually \c __LINE__).
/// \param function Source function (usually \c __func__).
/// \param message The message to output.

Y_API void abort(const StaticString &file, int line, const StaticString &function, const StaticString &message, ...) noexcept;

} // namespace Yttrium

/// \def Y_ABORT(condition)
/// \brief Terminate the program, outputting a message to the log.

#define Y_ABORT(...) Yttrium::abort(Y_S(__FILE__), __LINE__, Y_S(__func__), __VA_ARGS__)

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
				Y_ABORT(Y_S("Assertion failed: " #condition)); \
			} \
		} while (false)
#endif

#endif // __Y_ASSERT_H
