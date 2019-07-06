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

#include <yttrium/utils/flags.h>

#include "../texture.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlTexture2D final : public BackendTexture2D
	{
	public:
		GlTexture2D(RenderBackend&, const ImageInfo&, bool has_mipmaps, GlTextureHandle&&);

		void bind(Flags<Texture2D::Filter>) const;

	private:
		const GlTextureHandle _texture;
	};
}
