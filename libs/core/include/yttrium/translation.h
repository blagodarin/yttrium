// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>

namespace Yt
{
	class Source;
	class Writer;

	/// Translation file.
	class Translation
	{
	public:
		///
		/// May throw DataError.
		static std::unique_ptr<Translation> load(const Source&);

		virtual ~Translation() noexcept = default;

		///
		virtual void add(std::string_view source) = 0;

		///
		virtual bool changed() const noexcept = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual void save(Writer&&) const = 0;

		///
		virtual std::string translate(std::string_view source) const = 0;
	};
}
