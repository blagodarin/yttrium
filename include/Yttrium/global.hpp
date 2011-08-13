/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) by all other %Yttrium headers.

#ifndef __Y_GLOBAL_HPP
#define __Y_GLOBAL_HPP

#include <cstddef>  // NULL

/// \def Y_API
/// \brief %Yttrium API specifier.

#ifndef Y_API
	#define Y_API
#endif

/// \def Y_FUNC
/// \brief Pseudo-variable containing a current function name.

#if __GNUC__
	#define Y_FUNC __PRETTY_FUNCTION__
#elif _MSC_VER
	#define Y_FUNC __FUNCSIG__
#else
	#define Y_FUNC __func__
#endif

#define __Y_STR(x) #x

/// Stringification wrapper.

#define Y_STR(x) __Y_STR(x)

/// %Yttrium namespace.

namespace Yttrium
{
} // namespace Yttrium

#endif // __Y_GLOBAL_HPP
