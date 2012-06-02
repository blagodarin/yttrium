/// \file
/// \brief

#ifndef __Y_NONCOPYABLE_H
#define __Y_NONCOPYABLE_H

#include <Yttrium/global.h>

namespace Yttrium
{

///

class Noncopyable
{
public:

	Noncopyable() = default;

	Noncopyable(const Noncopyable &) = delete;

	Noncopyable &operator =(const Noncopyable &) = delete;
};

} // namespace Yttrium

#endif // __Y_NONCOPYABLE_H
