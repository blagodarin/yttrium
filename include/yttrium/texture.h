/// \file
/// \brief Textures.

#ifndef _include_yttrium_texture_h_
#define _include_yttrium_texture_h_

#include <yttrium/math/rect.h>
#include <yttrium/memory/shared_ptr.h>

namespace Yttrium
{
	/// 2D texture.
	class Y_API Texture2D : public Object
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

		///
		Filter filter() const { return _filter; }

		///
		Rect rect() const { return Rect(_size); }

		///
		void set_filter(Filter filter) { _filter = filter; }

		///
		Size size() const { return _size; }

	protected:
		const Size _size;
		Filter _filter = NearestFilter;

		Texture2D(const Size& size) : _size(size) {}
	};
}

#endif
