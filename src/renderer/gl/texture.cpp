#include "texture.h"

namespace Yttrium
{
	GlTexture2D::GlTexture2D(const ImageFormat& format, Allocator* allocator, const GlApi& gl, GLuint texture)
		: BackendTexture2D(format, allocator)
		, _gl(gl)
		, _texture(texture)
	{
	}

	GlTexture2D::~GlTexture2D()
	{
		_gl.DeleteTextures(1, &_texture);
	}

	void GlTexture2D::bind() const
	{
		GLenum min_filter;
		GLenum mag_filter;

		switch (_filter & Texture2D::IsotropicFilterMask)
		{
		case Texture2D::NearestFilter:

			min_filter = (_has_mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
			mag_filter = GL_NEAREST;
			break;

		case Texture2D::LinearFilter:

			min_filter = (_has_mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
			mag_filter = GL_NEAREST;
			break;

		case Texture2D::BilinearFilter:

			min_filter = (_has_mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
			mag_filter = GL_LINEAR;
			break;

		case Texture2D::TrilinearFilter:

			min_filter = (_has_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			mag_filter = GL_LINEAR;
			break;
		}

		_gl.BindTexture(GL_TEXTURE_2D, _texture);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

		if (_gl.EXT_texture_filter_anisotropic)
		{
			_gl.TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				(_filter & Texture2D::AnisotropicFilter ? _gl.MAX_TEXTURE_MAX_ANISOTROPY_EXT : 1.f));
		}
	}

	Vector2 GlTexture2D::fix_coords(const Vector2& coords) const
	{
		float x = coords.x;
		float y = coords.y;

		switch (_orientation)
		{
		case ImageOrientation::XRightYDown:

			break;

		case ImageOrientation::XRightYUp:

			y = _size.height - y;
			break;

		case ImageOrientation::XLeftYDown:

			x = _size.width - x;
			break;

		case ImageOrientation::XLeftYUp:

			x = _size.width - x;
			y = _size.height - y;
			break;
		}

		return Vector2(x / _size.width, y / _size.height);
	}
}
