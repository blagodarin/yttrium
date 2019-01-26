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

#include "texture.h"

#include <yttrium/math/rect.h>
#include "backend.h"

namespace Yttrium
{
	BackendTexture2D::BackendTexture2D(RenderBackend& backend, const ImageInfo& info, bool has_mipmaps)
		: _backend{ backend }
		, _size{ info.width(), info.height() }
		, _orientation{ info.orientation() }
		, _has_mipmaps{ has_mipmaps }
	{
	}

	RectF BackendTexture2D::full_rectangle() const
	{
		return _backend.map_rect({ {}, SizeF(_size) }, _orientation);
	}
}
