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

#ifndef _src_renderer_texture_h_
#define _src_renderer_texture_h_

#include <yttrium/renderer/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class RectF;
	class RenderBackend;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RenderBackend&, const ImageFormat&, bool has_mipmaps);

		Size size() const noexcept override { return _size; }

		RectF full_rectangle() const;
		ImageOrientation orientation() const { return _orientation; }

	protected:
		RenderBackend& _backend;
		const Size _size;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;
	};
}

#endif
