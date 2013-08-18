/// \file
/// \brief Textures.

#ifndef __Y_RENDERER_TEXTURE_H
#define __Y_RENDERER_TEXTURE_H

#include <yttrium/object.h>

namespace Yttrium
{

/// 2D texture.

class Y_API Texture2D: public Object
{
public:

	///

	typedef uint_fast8_t Filter;

	enum: Filter
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

public:

	///

	Filter filter() const noexcept { return _filter; }

	///

	void set_filter(Filter filter) noexcept { _filter = filter; }

protected:

	Texture2D(Allocator *allocator) noexcept: Object(allocator), _filter(NearestFilter) {}

protected:

	Filter _filter;
};

} // namespace Yttrium

#endif // __Y_RENDERER_TEXTURE_H
