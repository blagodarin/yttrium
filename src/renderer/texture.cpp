#include "texture.h"

namespace Yttrium
{

Texture2D::Private::Private(const TextureCache &cache, const ImageFormat &format, Allocator *allocator)
	: PrivateBase(allocator)
	, _cache(cache)
	, _size(format.width(), format.height())
	, _filter(Texture2D::NearestFilter)
	, _orientation(format.orientation())
	, _has_mipmaps(true)
{
}

void Texture2D::Private::set_filter(Texture2D::Filter filter)
{
	_filter = filter;
}

Texture2D::Texture2D(const Texture2D &texture)
	: _private(Private::copy(texture._private))
{
}

Texture2D::~Texture2D()
{
	Private::release(&_private);
}

Texture2D::Filter Texture2D::filter() const
{
	return _private->_filter;
}

void Texture2D::set_filter(Filter filter)
{
	_private->set_filter(filter);
}

Dim2 Texture2D::size() const
{
	return _private->_size;
}

Texture2D &Texture2D::operator =(const Texture2D &texture)
{
	Private::assign(&_private, texture._private);

	return *this;
}

} // namespace Yttrium
