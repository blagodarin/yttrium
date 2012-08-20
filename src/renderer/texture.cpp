#include "texture.h"

namespace Yttrium
{

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
	Private::copy(&_private, texture._private);

	return *this;
}

} // namespace Yttrium
