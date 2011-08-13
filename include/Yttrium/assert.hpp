/// \file
/// \brief Run-time assertions.

#ifndef __Y_ASSERT_HPP
#define __Y_ASSERT_HPP

#include <cassert>

#include <Yttrium/global.hpp>

namespace Yttrium
{

void Y_API abort(const char *message = NULL) throw();

} // namespace Yttrium

/// \def Y_ASSERT(condition)
/// \brief Terminate the program if the \a condition is \c false.

#ifndef __Y_DEBUG
	#define Y_ASSERT(condition)
#else
	#define Y_ASSERT(condition) \
		do \
		{ \
			if (!(condition)) \
				Yttrium::abort("Assertion failed: " #condition " (" __FILE__ ":" Y_STR(__LINE__) ")"); \
		} while (false)
#endif

#endif // __Y_ASSERT_HPP
