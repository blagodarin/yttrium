// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/numeric.h>

namespace Yt
{
#pragma pack(push, 1)

	struct YpFileHeader
	{
		uint32_t _signature = 0;
		uint32_t _reserved = 0;
		uint32_t _indexSize = 0;
		uint32_t _entryCount = 0;

		static constexpr auto kSignature = make_cc('\xDF', 'Y', 'P', '\xDA');
	};

	struct YpFileEntry
	{
		uint64_t _dataOffset = 0;
		uint32_t _dataSize = 0;
		uint32_t _metadataOffset = 0;
	};

#pragma pack(pop)
}
