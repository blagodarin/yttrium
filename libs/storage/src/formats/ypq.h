// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/buffer.h>
#include <yttrium/base/numeric.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/writer.h>

#include <vector>

namespace Yt
{
	constexpr auto kYpqSignature = make_cc('\xDF', 'Y', 'P', 'Q');

	class YpqReader final : public PackageReader
	{
	public:
		explicit YpqReader(std::unique_ptr<Source>&&);
		~YpqReader() override;

		const std::vector<std::string_view>& names() const override { return _names; }
		std::unique_ptr<Source> open(std::size_t) const override;

	private:
		struct Entry;

		const std::shared_ptr<const Source> _source;
		Buffer _metadata_buffer;
		std::vector<std::string_view> _names;
		std::vector<Entry> _entries;
	};

	class YpqWriter final : public PackageWriter
	{
	public:
		explicit YpqWriter(Writer&&);
		~YpqWriter() override;

		bool add(const std::string&) override;
		bool commit() override;

	private:
		struct Entry;

		Writer _writer;
		std::vector<Entry> _entries;
		bool _committed = false;
		bool _finished = false;
	};
}
