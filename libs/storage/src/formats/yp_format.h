// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_base/endian.hpp>

namespace Yt
{
	enum class YpCompression : uint8_t
	{
		None,
		Zlib,
	};

#pragma pack(push, 1)

	struct YpBlockEntry
	{
		uint32_t _compressedSize = 0;
		uint32_t _uncompressedSize = 0;
	};

	struct YpPackageHeader
	{
		static constexpr auto kSignature = seir::makeCC('\xDF', 'Y', 'P', '\x01'); // The last byte specifies format version.

		uint32_t _signature = kSignature;
		uint16_t _fileCount = 0;
		YpCompression _compression = YpCompression::None;
		uint8_t _reserved = 0;
		YpBlockEntry _indexBlock;
	};

#pragma pack(pop)

	static_assert(sizeof(YpPackageHeader) == 16);
	static_assert(sizeof(YpBlockEntry) == 8);
}
