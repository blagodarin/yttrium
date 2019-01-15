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

#include <cstddef>

namespace Yttrium
{
	constexpr size_t unaligned_image_row_size(size_t width, size_t bits_per_pixel)
	{
		return (width * bits_per_pixel + 7) / 8;
	}

	constexpr size_t aligned_image_row_size(size_t width, size_t bits_per_pixel, size_t row_alignment)
	{
		return (unaligned_image_row_size(width, bits_per_pixel) + row_alignment - 1) / row_alignment * row_alignment;
	}
}
