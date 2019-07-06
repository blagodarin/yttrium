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

namespace Yttrium
{
	class Size;

	/// 2D texture.
	class Texture2D
	{
	public:
		///
		enum Filter
		{
			// Base filter flags and masks.

			NearestTexelFilter = 0 << 0, ///< Nearest texel.
			LinearTexelFilter = 1 << 0,  ///< Linear texel.

			TexelFilterMask = 1 << 0, ///< Texel filter mask.

			NearestMipmapFilter = 0 << 1, ///< Nearest mipmap.
			LinearMipmapFilter = 1 << 1,  ///< Linear mipmap.

			MipmapFilterMask = 1 << 1, ///< Mipmap mask.

			IsotropicFilterMask = TexelFilterMask | MipmapFilterMask, ///< Isotropic mask.

			IsotropicFilter = 0 << 3,   ///< Isotropic filter.
			AnisotropicFilter = 1 << 3, ///< Anisotropic filter.

			// Derived filters.

			NearestFilter = NearestTexelFilter | NearestMipmapFilter, ///< Nearest filter.
			LinearFilter = NearestTexelFilter | LinearMipmapFilter,   ///< Linear filter.
			BilinearFilter = LinearTexelFilter | NearestMipmapFilter, ///< Bilinear filter.
			TrilinearFilter = LinearTexelFilter | LinearMipmapFilter, ///< Trilinear filter.
		};

		///
		virtual ~Texture2D() = default;

		///
		virtual Size size() const noexcept = 0;
	};
}
