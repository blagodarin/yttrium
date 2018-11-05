#ifndef _libs_yttrium_utils_fourcc_h_
#define _libs_yttrium_utils_fourcc_h_

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	inline namespace Literals
	{
		constexpr uint16_t operator"" _twocc(const char* text, size_t)
		{
			return static_cast<uint16_t>(static_cast<unsigned char>(text[0]) | static_cast<unsigned char>(text[1]) << 8);
		}

		constexpr uint32_t operator"" _fourcc(const char* text, size_t)
		{
			return uint32_t{static_cast<unsigned char>(text[0])}
				| uint32_t{static_cast<unsigned char>(text[1])} << 8
				| uint32_t{static_cast<unsigned char>(text[2])} << 8 * 2
				| uint32_t{static_cast<unsigned char>(text[3])} << 8 * 3
				;
		}

		constexpr uint64_t operator"" _eightcc(const char* text, size_t)
		{
			return uint64_t{static_cast<unsigned char>(text[0])}
				| uint64_t{static_cast<unsigned char>(text[1])} << 8
				| uint64_t{static_cast<unsigned char>(text[2])} << 8 * 2
				| uint64_t{static_cast<unsigned char>(text[3])} << 8 * 3
				| uint64_t{static_cast<unsigned char>(text[4])} << 8 * 4
				| uint64_t{static_cast<unsigned char>(text[5])} << 8 * 5
				| uint64_t{static_cast<unsigned char>(text[6])} << 8 * 6
				| uint64_t{static_cast<unsigned char>(text[7])} << 8 * 7
				;
		}
	}

	static_assert("\x00\xff"_twocc == 0xff00);
	static_assert("\x00\x80\x7f\xff"_fourcc == 0xff7f8000);
	static_assert("\x00\x01\x7e\x7f\x80\x81\xfe\xff"_eightcc == 0xfffe81807f7e0100);
}

#endif