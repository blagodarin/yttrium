/// \file
/// \brief

#ifndef __Y_NONCOPYABLE_HPP
#define __Y_NONCOPYABLE_HPP

#include <Yttrium/global.hpp>

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

#endif // __Y_NONCOPYABLE_HPP
