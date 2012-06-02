/// \file
/// \brief ION Object Notation (ION) format facilities.

#ifndef __Y_ION_H
#define __Y_ION_H

#include <Yttrium/ion/document.h>
#include <Yttrium/ion/list.h>
#include <Yttrium/ion/node.h>
#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>

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

#endif // __Y_ION_H
