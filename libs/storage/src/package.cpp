// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/package.h>

#include <yttrium/base/exceptions.h>
#include <yttrium/storage/source.h>
#include "formats/yp_format.h"
#include "formats/yp_reader.h"
#include "package.h"

#include <cassert>

namespace Yt
{
	std::unique_ptr<PackageReader> PackageReader::create(std::unique_ptr<Source>&& source)
	{
		assert(source);
		uint32_t magic = 0;
		if (source->read_at(0, magic) && magic == YpPackageHeader::kSignature)
			return std::make_unique<YpReader>(std::move(source));
		throw DataError{ "Unknown package format" };
	}
}
