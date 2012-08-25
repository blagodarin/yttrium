/// \file
/// \brief

#ifndef __Y_RENDERER_TEXTURE_H
#define __Y_RENDERER_TEXTURE_H

#include <Yttrium/vector.h>

namespace Yttrium
{

class Renderer;
class TextureCache;

///

class Y_API Texture2D
{
	friend class Renderer;
	friend class TextureCache;

public:

	///

	typedef uint_fast8_t Filter;

	enum
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

	inline Texture2D() noexcept;

	///

	Texture2D(const Texture2D &texture) noexcept;

	///

	~Texture2D() noexcept;

public:

	///

	Filter filter() const noexcept;

	///

	Dim2 size() const noexcept;

	///

	void set_filter(Filter filter) noexcept;

public:

	///

	inline operator bool() const noexcept;

	///

	inline bool operator ==(const Texture2D &texture) noexcept;

	///

	inline bool operator !=(const Texture2D &texture) noexcept;

	///

	Texture2D &operator =(const Texture2D &texture) noexcept;

public:

	class Private;

private:

	inline Texture2D(Private *private_);

private:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////

Texture2D::Texture2D() noexcept
	: _private(nullptr)
{
}

Texture2D::operator bool() const noexcept
{
	return _private;
}

bool Texture2D::operator ==(const Texture2D &texture) noexcept
{
	return _private == texture._private;
}

bool Texture2D::operator !=(const Texture2D &texture) noexcept
{
	return _private != texture._private;
}

Texture2D::Texture2D(Private *private_)
	: _private(private_)
{
}

} // namespace Yttrium

#endif // __Y_RENDERER_TEXTURE_H
