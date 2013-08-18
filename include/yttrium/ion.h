/// \file
/// \brief ION Object Notation (ION) format facilities.

#ifndef __Y_ION_H
#define __Y_ION_H

#include <yttrium/ion/document.h>
#include <yttrium/ion/list.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>

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
