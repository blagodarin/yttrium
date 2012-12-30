#include "texture_cache.h"

#include <Yttrium/utils.h>

#include "texture.h"

namespace Yttrium
{

Texture2D::Private *OpenGlTextureCache::cache_texture_2d(const TextureCache &cache, const ImageFormat &format, const void *data, Allocator *allocator)
{
	// NOTE: Keep the new pixel formats in sync with these arrays!

	static const GLint internal_formats[] =
	{
		GL_LUMINANCE8,        // PixelFormat::Gray
		GL_LUMINANCE8_ALPHA8, // PixelFormat::GrayAlpha
		GL_LUMINANCE8_ALPHA8, // PixelFormat::AlphaGray
		GL_RGB8,              // PixelFormat::Rgb
		GL_RGB8,              // PixelFormat::Bgr
		GL_RGBA8,             // PixelFormat::Rgba
		GL_RGBA8,             // PixelFormat::Bgra
		GL_RGBA8,             // PixelFormat::Argb
		GL_RGBA8,             // PixelFormat::Abgr
	};

	static const GLenum formats[] =
	{
		GL_LUMINANCE,       // PixelFormat::Gray
		GL_LUMINANCE_ALPHA, // PixelFormat::GrayAlpha
		0,                  // PixelFormat::AlphaGray
		GL_RGB,             // PixelFormat::Rgb
		GL_BGR,             // PixelFormat::Bgr
		GL_RGBA,            // PixelFormat::Rgba
		GL_BGRA,            // PixelFormat::Bgra
		0,                  // PixelFormat::Argb
		0,                  // PixelFormat::Abgr
	};

	GLenum data_format = formats[size_t(format.pixel_format())];
	GLint internal_format = internal_formats[size_t(format.pixel_format())];

	if (format.bits_per_channel() == 8 && data_format)
	{
		GLenum target = 0;

		if (_gl.ARB_texture_non_power_of_two || (is_power_of_2(format.width()) && is_power_of_2(format.height())))
		{
			target = GL_TEXTURE_2D;
		}
		else if (_gl.ARB_texture_rectangle)
		{
			target = GL_TEXTURE_RECTANGLE_ARB;
		}

		if (target)
		{
			GLuint texture;

			_gl.GenTextures(1, &texture); // TODO: Think of using Y_ABORT if this fails.
			if (texture)
			{
				_gl.BindTexture(target, texture);
				_gl.Hint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
				_gl.TexImage2D(target, 0, internal_format, format.width(), format.height(), 0, data_format, GL_UNSIGNED_BYTE, data);

				GLboolean is_target_enabled = _gl.IsEnabled(target);
				_gl.Enable(target); // ATI bug workaround, see [http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation].
				_gl.GenerateMipmap(target);
				if (!is_target_enabled)
				{
					_gl.Disable(target);
				}

				return Y_NEW(allocator, OpenGlTexture2D)(cache, format, allocator, _gl, target, texture);
			}
		}
	}

	return nullptr;
}

} // namespace Yttrium
