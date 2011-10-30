/// \file
/// \brief Safe bool.

#ifndef __Y_SAFE_BOOL_HPP
#define __Y_SAFE_BOOL_HPP

#include <Yttrium/global.hpp>

namespace Yttrium
{

class Y_API __SafeBool
{
public:

	typedef void (__SafeBool::*Type)();

	void value();
};

/// Safe boolean type.

typedef __SafeBool::Type SafeBool;

} // namespace Yttrium

/// \def Y_SAFE_FALSE
/// \brief Safe boolean \c false.

#define Y_SAFE_FALSE (SafeBool(NULL))

/// \def Y_SAFE_TRUE
/// \brief Safe boolean \c true.

#define Y_SAFE_TRUE (&Yttrium::__SafeBool::value)

/// Safe boolean conversion helper.

#define Y_SAFE_BOOL(x) ((x) ? Y_SAFE_TRUE : Y_SAFE_FALSE)

#endif // __Y_SAFE_BOOL_HPP
