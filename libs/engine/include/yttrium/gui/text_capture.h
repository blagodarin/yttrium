//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/math/rect.h>

namespace Yttrium
{
	class TextCapture
	{
	public:
		const std::size_t _cursor_pos;
		bool _has_cursor = false;
		RectF _cursor_rect;

		const std::size_t _selection_begin;
		const std::size_t _selection_end;
		bool _has_selection = false;
		RectF _selection_rect;

		constexpr TextCapture(std::size_t cursor_pos, std::size_t selection_begin, std::size_t selection_size) noexcept
			: _cursor_pos{ cursor_pos }, _selection_begin{ selection_begin }, _selection_end{ _selection_begin + selection_size } {}
	};
}
