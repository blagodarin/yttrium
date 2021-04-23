// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

namespace Yt
{
	class Writer;

	// Yttrium Package (YP) file writer.
	class YpWriter
	{
	public:
		enum class Compression
		{
			None,
			Zlib,
		};

		explicit YpWriter(Writer&&, Compression);
		~YpWriter() noexcept;

		bool add(const std::string&, int compressionLevel = 0);
		bool commit();

	private:
		const std::unique_ptr<struct YpWriterData> _data;
	};
}
