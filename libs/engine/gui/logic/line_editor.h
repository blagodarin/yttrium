//
// Copyright 2019 Sergei Blagodarin
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

#include <string>

namespace Yttrium
{
	class KeyEvent;

	class LineEditor
	{
	public:
		void clear() noexcept;
		auto cursor() const noexcept { return _cursor; }
		void insert(std::string_view);
		bool process_key(const KeyEvent&);
		void reset(std::string&&) noexcept;
		auto selection_offset() const noexcept { return _selection_offset; }
		auto selection_size() const noexcept { return _selection_size; }
		void set_max_bytes(std::size_t) noexcept;
		auto& text() const noexcept { return _text; }

	private:
		std::size_t left_codepoint_bytes() const noexcept;
		std::size_t right_codepoint_bytes() const noexcept;

	private:
		std::string _text;
		std::size_t _cursor = 0;
		std::size_t _selection_size = 0;
		std::size_t _selection_offset = 0;
		std::size_t _max_bytes = _text.max_size();
	};
}
