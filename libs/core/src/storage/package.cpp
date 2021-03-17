// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/package.h>

#include <yttrium/logger.h>
#include <yttrium/storage/source.h>
#include "../utils/string.h"
#include "formats/ypq.h"
#include "package.h"

#include <fmt/format.h>

namespace Yt
{
	std::unique_ptr<PackageReader> PackageReader::create(const std::filesystem::path& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (path.extension() == ".ypq")
				type = PackageType::Ypq;
			else
				return {};
		}
		auto source = Source::from(path);
		if (!source)
			return {};
		try
		{
			if (type == PackageType::Ypq)
				return std::make_unique<YpqReader>(std::move(source));
		}
		catch (const BadPackage& e)
		{
			Logger::write(fmt::format("({}) {}", path.string(), e.what()));
		}
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
