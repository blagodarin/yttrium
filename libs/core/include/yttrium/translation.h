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

#pragma once

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Source;
	class Writer;

	/// Translation file.
	class Y_CORE_API Translation
	{
	public:
		///
		/// May throw DataError.
		static std::unique_ptr<Translation> load(const Source&);

		virtual ~Translation() = default;

		///
		virtual void add(std::string_view source) = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual void save(Writer&&) const = 0;

		///
		virtual std::string translate(std::string_view source) const = 0;
	};
}
