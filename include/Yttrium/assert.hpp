/// \file
/// \brief Run-time assertions.

#ifndef __Y_ASSERT_HPP
#define __Y_ASSERT_HPP

#include <Yttrium/global.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

/// Terminate the program, outputting a message to the log.
/// \param message The message to output.
/// \param file Source file name (usually \c __FILE__).
/// \param line Source file line number (usually \c __LINE__).
/// \param function Source function (usually \c __func__).

Y_API void abort(const StaticString &message, const StaticString &file, int line, const StaticString &function) throw();

} // namespace Yttrium

/// \def Y_ABORT(condition)
/// \brief Terminate the program, outputting a message to the log.

#define Y_ABORT(message) Yttrium::abort(message, Y_S(__FILE__), __LINE__, Y_S(__func__))

/// \def Y_ASSERT(condition)
/// \brief Terminate the program if the \a condition is \c false.

#ifndef __Y_DEBUG
	#define Y_ASSERT(condition)
#else
	#define Y_ASSERT(condition) \
		do \
		{ \
			if (!(condition)) \
			{ \
				Y_ABORT(Y_S("Assertion failed: " #condition)); \
			} \
		} while (false)
#endif

#endif // __Y_ASSERT_HPP
