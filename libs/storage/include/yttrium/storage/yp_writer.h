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
		explicit YpWriter(Writer&&);
		~YpWriter() noexcept;

		bool add(const std::string&);
		bool commit();

	private:
		const std::unique_ptr<struct YpWriterData> _data;
	};
}
