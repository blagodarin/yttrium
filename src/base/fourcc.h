#ifndef _src_base_fourcc_h_
#define _src_base_fourcc_h_

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	constexpr uint32_t operator"" _fourcc(const char* text, size_t)
	{
		return static_cast<uint32_t>(text[0])
			| static_cast<uint32_t>(text[1]) << 8
			| static_cast<uint32_t>(text[2]) << 16
			| static_cast<uint32_t>(text[3]) << 24;
	}

	constexpr uint64_t operator"" _eightcc(const char* text, size_t)
	{
		return static_cast<uint64_t>(text[0])
			| static_cast<uint64_t>(text[1]) << 8
			| static_cast<uint64_t>(text[2]) << 16
			| static_cast<uint64_t>(text[3]) << 24
			| static_cast<uint64_t>(text[4]) << 32
			| static_cast<uint64_t>(text[5]) << 40
			| static_cast<uint64_t>(text[6]) << 48
			| static_cast<uint64_t>(text[7]) << 56;
	}

	static_assert("ABCD"_fourcc == 0x44434241, "FOURCC compile time test failed");
	static_assert("ABCDEFGH"_eightcc == 0x4847464544434241, "EIGHTCC compile time test failed");
}

#endif
