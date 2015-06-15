#ifndef __BASE_FOURCC_H
#define __BASE_FOURCC_H

#include <cstdint>

namespace Yttrium
{
	template <uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3>
	class Fourcc
	{
	public:

		static const uint32_t value = c0
			| uint32_t{c1} << 8
			| uint32_t{c2} << 16
			| uint32_t{c3} << 24;
	};

	template <uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7>
	class Eightcc
	{
	public:

		static const uint64_t value = c0
			| uint64_t{c1} << 8
			| uint64_t{c2} << 16
			| uint64_t{c3} << 24
			| uint64_t{c4} << 32
			| uint64_t{c5} << 40
			| uint64_t{c6} << 48
			| uint64_t{c7} << 56;
	};
}

#endif // __BASE_FOURCC_H
