/// \file
/// \brief Textures.

#ifndef _include_yttrium_renderer_texture_h_
#define _include_yttrium_renderer_texture_h_

#include <yttrium/utils/flags.h>

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

			NearestTexelFilter  = 0 << 0, ///< Nearest texel.
			LinearTexelFilter   = 1 << 0, ///< Linear texel.

			TexelFilterMask     = 1 << 0, ///< Texel filter mask.

			NearestMipmapFilter = 0 << 1, ///< Nearest mipmap.
			LinearMipmapFilter  = 1 << 1, ///< Linear mipmap.

			MipmapFilterMask    = 1 << 1, ///< Mipmap mask.

			IsotropicFilterMask = TexelFilterMask | MipmapFilterMask, ///< Isotropic mask.

			IsotropicFilter     = 0 << 3, ///< Isotropic filter.
			AnisotropicFilter   = 1 << 3, ///< Anisotropic filter.

			// Derived filters.

			NearestFilter   = NearestTexelFilter | NearestMipmapFilter, ///< Nearest filter.
			LinearFilter    = NearestTexelFilter | LinearMipmapFilter,  ///< Linear filter.
			BilinearFilter  = LinearTexelFilter  | NearestMipmapFilter, ///< Bilinear filter.
			TrilinearFilter = LinearTexelFilter  | LinearMipmapFilter,  ///< Trilinear filter.
		};

		///
		virtual ~Texture2D() = default;

		///
		virtual Size size() const noexcept = 0;
	};
}

#endif
