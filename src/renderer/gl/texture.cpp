#include "texture.h"

#include <stdexcept>

namespace Yttrium
{
	GlTextureHandle::GlTextureHandle(const GlApi& gl, GLenum target)
		: _gl(gl)
		, _target(target)
	{
		_gl.GenTextures(1, &_handle);
		if (!_handle)
			throw std::runtime_error("glGenTextures failed");
	}

	GlTextureHandle::GlTextureHandle(GlTextureHandle&& handle)
		: _gl(handle._gl)
		, _target(handle._target)
		, _handle(handle._handle)
	{
		handle._handle = 0;
	}

	GlTextureHandle::~GlTextureHandle()
	{
		if (_handle)
			_gl.DeleteTextures(1, &_handle);
	}

	void GlTextureHandle::bind() const
	{
		_gl.BindTexture(_target, _handle);
	}

	void GlTextureHandle::set_anisotropy_enabled(bool enabled) const
	{
		if (_gl.EXT_texture_filter_anisotropic)
			_gl.TexParameterf(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, enabled ? _gl.MAX_TEXTURE_MAX_ANISOTROPY_EXT : 1.f);
	}

	void GlTextureHandle::set_parameter(GLenum name, GLint value) const
	{
		_gl.TexParameteri(_target, name, value);
	}

	void GlTextureHandle::unbind() const
	{
		_gl.BindTexture(_target, 0);
	}

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

		_texture.bind();
		_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		_texture.set_parameter(GL_TEXTURE_MIN_FILTER, min_filter);
		_texture.set_parameter(GL_TEXTURE_MAG_FILTER, mag_filter);
		_texture.set_anisotropy_enabled(_filter & Texture2D::AnisotropicFilter);
	}

	Vector2 GlTexture2D::map(const Vector2& coords) const
	{
		auto x = coords.x;
		auto y = coords.y;
		if (_orientation == ImageOrientation::XLeftYDown || _orientation == ImageOrientation::XLeftYUp)
			x = _size.width - x;
		if (_orientation == ImageOrientation::XRightYUp || _orientation == ImageOrientation::XLeftYUp)
			y = _size.height - y;
		return Vector2(x / _size.width, y / _size.height);
	}
}
