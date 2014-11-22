#ifndef __BASE_FOURCC_H
#define __BASE_FOURCC_H

#include <yttrium/global.h>

#include <cstdint>

namespace Yttrium
{

// TODO: Add big endian FOURCC support should a BE target platform appear.

template <unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3>
class Fourcc
{
public:

	static const uint32_t value = c0 | static_cast<uint32_t>(c1) << 8
		| static_cast<uint32_t>(c2) << 16 | static_cast<uint32_t>(c3) << 24;
};

template <unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3,
	unsigned char c4, unsigned char c5, unsigned char c6, unsigned char c7>
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
