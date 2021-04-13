// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/storage/package.h>

#include <primal/buffer.hpp>

#include <vector>

namespace Yt
{
	class YpReader final : public PackageReader
	{
	public:
		explicit YpReader(std::unique_ptr<Source>&&);
		~YpReader() override;

		const std::vector<std::string_view>& names() const override { return _names; }
		std::unique_ptr<Source> open(std::size_t) const override;

	private:
		struct Entry;

		const std::shared_ptr<const Source> _source;
		primal::Buffer<char> _metadataBuffer;
		std::vector<std::string_view> _names;
		std::vector<Entry> _entries;
	};
}
