#include "texture.h"

#include <utility>

namespace Yttrium
{
	GlTexture2D::GlTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps, GlTextureHandle&& texture)
		: BackendTexture2D(renderer, format, has_mipmaps)
		, _texture(std::move(texture))
	{
	}

	void GlTexture2D::bind() const
	{
		GLint min_filter = GL_NEAREST;
		GLint mag_filter = GL_NEAREST;

		switch (_filter & Texture2D::IsotropicFilterMask)
		{
		case Texture2D::NearestFilter:
			min_filter = _has_mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			mag_filter = GL_NEAREST;
			break;

		case Texture2D::LinearFilter:
			min_filter = _has_mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
			mag_filter = GL_NEAREST;
			break;

		case Texture2D::BilinearFilter:
			min_filter = _has_mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
			mag_filter = GL_LINEAR;
			break;

		case Texture2D::TrilinearFilter:
			min_filter = _has_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			mag_filter = GL_LINEAR;
			break;
		}

		_texture.bind(); // TODO: Separate texture binding and parameter modification.
		_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		_texture.set_parameter(GL_TEXTURE_MIN_FILTER, min_filter);
		_texture.set_parameter(GL_TEXTURE_MAG_FILTER, mag_filter);
		_texture.set_anisotropy_enabled(_filter & Texture2D::AnisotropicFilter);
	}

	PointF GlTexture2D::map(const PointF& coords) const
	{
		auto x = coords.x();
		auto y = coords.y();
		if (_orientation == ImageOrientation::XLeftYDown || _orientation == ImageOrientation::XLeftYUp)
			x = _size.width() - x;
		if (_orientation == ImageOrientation::XRightYUp || _orientation == ImageOrientation::XLeftYUp)
			y = _size.height() - y;
		return {x / _size.width(), y / _size.height()};
	}

	PointF GlTexture2D::map_scaled(const PointF& coords) const
	{
		return PointF(
			_orientation == ImageOrientation::XLeftYDown || _orientation == ImageOrientation::XLeftYUp ? 1.f - coords.x() : coords.x(),
			_orientation == ImageOrientation::XRightYUp || _orientation == ImageOrientation::XLeftYUp ? 1.f - coords.y() : coords.y());
	}
}
