//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/storage/package.h>

#include <yttrium/storage/source.h>
#include "../utils/string.h"
#include "formats/ypq.h"
#include "package.h"

#include <iostream>
#include <string>

namespace Yttrium
{
	std::unique_ptr<PackageReader> PackageReader::create(const std::string& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (ends_with(path, ".ypq"))
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
			std::cerr << "(" << path << ") " << e.what() << "\n";
		}
		return {};
	}

	std::unique_ptr<PackageWriter> PackageWriter::create(const std::string& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (ends_with(path, ".ypq"))
				type = PackageType::Ypq;
			else
				return {};
		}
		Writer writer(path);
		if (!writer)
			return {};
		if (type == PackageType::Ypq)
			return std::make_unique<YpqWriter>(std::move(writer));
		return {};
	}
}
