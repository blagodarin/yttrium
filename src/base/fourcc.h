/// \file
/// \brief

#ifndef __BASE_FOURCC_H
#define __BASE_FOURCC_H

#include <Yttrium/types.h>

namespace Yttrium
{

// TODO: Add big endian FOURCC support should a BE target platform appear.

/// Four character code (FOURCC) construction helper.

template <UChar c0, UChar c1, UChar c2, UChar c3>
class Fourcc
{
public:

	static const uint32_t value = c0 | static_cast<uint32_t>(c1) << 8
		| static_cast<uint32_t>(c2) << 16 | static_cast<uint32_t>(c3) << 24;
};

/// Eight character code (EIGHTCC) construction helper.

template <UChar c0, UChar c1, UChar c2, UChar c3, UChar c4, UChar c5, UChar c6, UChar c7>
class Eightcc
{
public:

	static const uint64_t value = c0 | static_cast<uint64_t>(c1) << 8
		| static_cast<uint64_t>(c2) << 16 | static_cast<uint64_t>(c3) << 24
		| static_cast<uint64_t>(c4) << 32 | static_cast<uint64_t>(c5) << 40
		| static_cast<uint64_t>(c6) << 48 | static_cast<uint64_t>(c7) << 56;
};

} // namespace Yttrium

#endif // __BASE_FOURCC_H
