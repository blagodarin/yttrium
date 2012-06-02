/// \file
/// \brief

#ifndef __BASE_FOURCC_H
#define __BASE_FOURCC_H

#include <Yttrium/types.h>

namespace Yttrium
{

/// Four character code (FOURCC) construction helper.

template <char c0, char c1, char c2, char c3>
class Fourcc
{
public:

	// NOTE: This is little-endian only!

	static const uint32_t value = c0 | static_cast<uint32_t>(c1) << 8
		| static_cast<uint32_t>(c2) << 16 | static_cast<uint32_t>(c3) << 24;
};

/// Eight character code (EIGHTCC) construction helper.

template <char c0, char c1, char c2, char c3, char c4, char c5, char c6, char c7>
class Eightcc
{
public:

	// NOTE: This is little-endian only!

	static const uint64_t value = c0 | static_cast<uint64_t>(c1) << 8
		| static_cast<uint64_t>(c2) << 16 | static_cast<uint64_t>(c3) << 24
		| static_cast<uint64_t>(c4) << 32 | static_cast<uint64_t>(c5) << 40
		| static_cast<uint64_t>(c6) << 48 | static_cast<uint64_t>(c7) << 56;
};

} // namespace Yttrium

#endif // __BASE_FOURCC_H
