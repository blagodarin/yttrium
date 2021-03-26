// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/package.h>

#include <yttrium/exceptions.h>
#include <yttrium/storage/source.h>
#include "formats/ypq.h"
#include "package.h"

#include <fmt/format.h>

#include <cassert>

namespace Yt
{
	std::unique_ptr<PackageReader> PackageReader::create(std::unique_ptr<Source>&& source, PackageType type)
	{
		assert(source);
		if (type == PackageType::Auto)
		{
			uint32_t magic = 0;
			if (!source->read_at(0, magic))
				return {};
			if (magic == kYpqSignature)
				type = PackageType::Ypq;
			else
				throw DataError{ "Unknown package format" }; // TODO: Report this error with package source name.
		}
		if (type == PackageType::Ypq)
			return std::make_unique<YpqReader>(std::move(source));
		return {};
	}

	std::unique_ptr<PackageWriter> PackageWriter::create(const std::filesystem::path& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (path.extension() == ".ypq")
				type = PackageType::Ypq;
			else
				return {};
		}
		Writer writer{ path };
		if (!writer)
			return {};
		if (type == PackageType::Ypq)
			return std::make_unique<YpqWriter>(std::move(writer));
		return {};
	}
}
