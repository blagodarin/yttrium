#include "renderer.h"

#include <yttrium/matrix.h>

#include "../builtin_data.h"
#include "texture_cache.h"

namespace Yttrium
{

OpenGlRenderer::OpenGlRenderer(WindowBackend& window, Allocator* allocator)
	: Private(window, allocator)
{
	_gl.initialize(window);
}

OpenGlRenderer::~OpenGlRenderer()
{
	_gl.DeleteTextures(1, &_debug_texture);
}

bool OpenGlRenderer::initialize()
{
	if (!check_min_version(1, 2))
		return false; // TODO: Report error.

	_gl.Enable(GL_TEXTURE_2D);
	_gl.Enable(GL_BLEND);
	_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_gl.Hint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	_gl.GenTextures(1, &_debug_texture);
	if (!_debug_texture)
		return false; // TODO: Report error.
	_gl.BindTexture(GL_TEXTURE_2D, _debug_texture);
	_gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Builtin::texture_width, Builtin::texture_height,
		0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Builtin::texture);
	_gl.BindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void OpenGlRenderer::bind_debug_texture()
{
	_gl.BindTexture(GL_TEXTURE_2D, _debug_texture);
	_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void OpenGlRenderer::clear()
{
	_gl.ClearColor(0.5, 0.5, 0.5, 0);
	_gl.ClearDepth(1);
	_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

std::unique_ptr<TextureCache> OpenGlRenderer::create_texture_cache(Renderer& renderer)
{
	return std::make_unique<GlTextureCache>(renderer, _gl);
}

void OpenGlRenderer::flush_2d()
{
	_gl.EnableClientState(GL_COLOR_ARRAY);
	_gl.ColorPointer(4, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].color);

	if (_debug_rendering || _texture)
	{
		_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
		_gl.TexCoordPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].texture);
	}

	_gl.EnableClientState(GL_VERTEX_ARRAY);
	_gl.VertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].position);

	_gl.DrawElements(GL_TRIANGLE_STRIP, _indices_2d.size(), GL_UNSIGNED_SHORT, &_indices_2d[0]);

	_gl.DisableClientState(GL_VERTEX_ARRAY);
	_gl.DisableClientState(GL_COLOR_ARRAY);
	_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void OpenGlRenderer::set_matrix_2d(double width, double height)
{
	_gl.MatrixMode(GL_PROJECTION);
	_gl.LoadMatrixf(Matrix4f::projection_2d(0, 0, width, height).data());

	_gl.MatrixMode(GL_MODELVIEW);
	_gl.LoadMatrixf(Matrix4f().data());
}

void OpenGlRenderer::set_viewport(const Dim2 &size)
{
	GLint unpack_alignment;

	_gl.Viewport(0, 0, size.x, size.y);
	_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);

	Private::set_viewport(size);
	_screenshot_image.set_size(size.x, size.y, unpack_alignment);
}

void OpenGlRenderer::take_screenshot()
{
	GLint read_buffer;

	_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
	_gl.ReadBuffer(GL_FRONT);
	_gl.ReadPixels(0, 0, _viewport_size.x, _viewport_size.y, GL_RGB, GL_UNSIGNED_BYTE, _screenshot_image.data());
	_gl.ReadBuffer(read_buffer);
}

bool OpenGlRenderer::check_min_version(int major, int minor)
{
	// NOTE: I believe that the version number is in form "<one digit>.<one digit><anything>".
	const int actual_major = _gl.VERSION[0] - '0';
	const int actual_minor = _gl.VERSION[2] - '0';
	return actual_major > major || (actual_major == major && actual_minor >= minor);
}

} // namespace Yttrium
