// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/storage/package.h>

#include <seir_base/buffer.hpp>
#include <seir_base/rigid_vector.hpp>

#include <vector>

namespace Yt
{
	class Decompressor;

	class YpReader final : public PackageReader
	{
	public:
		explicit YpReader(std::unique_ptr<Source>&&);
		~YpReader() override;

		std::span<const std::string_view> names() const override { return { _names.data(), _names.size() }; }
		std::unique_ptr<Source> open(std::size_t) const override;

	private:
		struct Entry;

		const std::shared_ptr<const Source> _source;
		seir::RigidVector<Entry> _entries;
		seir::RigidVector<std::string_view> _names;
		seir::Buffer<uint8_t> _indexBuffer; // TODO: Store only metadata part of the index.
		std::unique_ptr<Decompressor> _decompressor;
	};
}
