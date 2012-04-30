/// \file
/// \brief ION Object Notation (ION) format facilities.

#ifndef __Y_ION_HPP
#define __Y_ION_HPP

#include <Yttrium/ion/document.hpp>
#include <Yttrium/ion/list.hpp>
#include <Yttrium/ion/node.hpp>
#include <Yttrium/ion/object.hpp>
#include <Yttrium/ion/value.hpp>

namespace Yttrium
{

/// ION facilities.

namespace Ion
{

enum
{
	DontIndent = -1, ///< Don't indent at all, outputting the shortest form possible.
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_HPP
