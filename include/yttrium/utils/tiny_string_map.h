//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _include_yttrium_utils_tiny_string_map_h_
#define _include_yttrium_utils_tiny_string_map_h_

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	///
	class Y_CORE_API TinyStringMap
	{
	public:
		TinyStringMap() noexcept = default;
		TinyStringMap(const TinyStringMap&);
		TinyStringMap(TinyStringMap&&) noexcept;
		TinyStringMap& operator=(const TinyStringMap&);
		TinyStringMap& operator=(TinyStringMap&&) noexcept;

		/// Removes the element with the specified key.
		void erase(std::string_view key) noexcept;

		///
		std::string_view find(std::string_view key) const noexcept;

		///
		void insert_or_assign(std::string_view key, std::string_view value);

		/// Reserves the specified number of bytes for the data buffer.
		void reserve_bytes(size_t);

	private:
		std::unique_ptr<uint8_t[]> _data;
		size_t _capacity = 0;
		size_t _size = 0;
	};
}

#endif
