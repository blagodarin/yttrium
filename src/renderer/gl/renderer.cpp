#include "renderer.h"

#include "../builtin/data.h"

namespace Yttrium
{

OpenGlRenderer::OpenGlRenderer(Window *window, Allocator *allocator)
	: Private(window, allocator)
	, _builtin_texture(0)
{
	_gl.initialize(window);

	if (!check_version(1, 2))
	{
		Y_ABORT("Invalid OpenGL version, expected 1.2 or higher"); // TODO: Be more polite.
	}

	_gl.Enable(GL_TEXTURE_2D);
	_gl.Enable(GL_BLEND);
	_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_gl.Hint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// TODO: Make builtin texture initialization more gentle.

	_gl.GenTextures(1, &_builtin_texture);

	if (!_builtin_texture)
	{
		Y_ABORT("Can't create builtin texture");
	}

	_gl.BindTexture(GL_TEXTURE_2D, _builtin_texture);
	_gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Builtin::texture_width, Builtin::texture_height,
		0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Builtin::texture);
	_gl.BindTexture(GL_TEXTURE_2D, 0);
}

OpenGlRenderer::~OpenGlRenderer()
{
	if (_builtin_texture)
	{
		_gl.DeleteTextures(1, &_builtin_texture);
	}
}

void OpenGlRenderer::bind_builtin()
{
	_gl.BindTexture(GL_TEXTURE_2D, _builtin_texture);

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

void OpenGlRenderer::flush_2d()
{
	_gl.EnableClientState(GL_COLOR_ARRAY);
	_gl.ColorPointer(4, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].color);

	if (_builtin._is_bound) // TODO: Check an ordinary texture too.
	{
		_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
		_gl.TexCoordPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].texture);
	}

	_gl.EnableClientState(GL_VERTEX_ARRAY);
	_gl.VertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].position);

	_gl.DrawElements(GL_QUADS, _indices_2d.size(), GL_UNSIGNED_SHORT, &_indices_2d[0]);

	_gl.DisableClientState(GL_VERTEX_ARRAY);
	_gl.DisableClientState(GL_COLOR_ARRAY);
	_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);

	_vertices_2d.clear();
	_indices_2d.clear();
}

void OpenGlRenderer::set_matrix_2d(double width, double height)
{
	_gl.MatrixMode(GL_PROJECTION);
	_gl.LoadIdentity();
	_gl.Ortho(0, width, height, 0, 0, 1);
	_gl.MatrixMode(GL_MODELVIEW);
	_gl.LoadIdentity();
}

void OpenGlRenderer::set_viewport(const Dim2 &size)
{
	_gl.Viewport(0, 0, size.width, size.height);
	Private::set_viewport(size);
}

void OpenGlRenderer::take_screenshot()
{
	GLint read_buffer;
	GLint unpack_alignment;

	_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
	_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
	_gl.ReadBuffer(GL_FRONT);
	_gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	_gl.ReadPixels(0, 0, _viewport_size.width, _viewport_size.height, GL_RGB, GL_UNSIGNED_BYTE, _screenshot_buffer.data());
	_gl.PixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
	_gl.ReadBuffer(read_buffer);
}

bool OpenGlRenderer::check_version(int major, int minor)
{
	// NOTE: I believe that the version number is in form "<one digit>.<one digit><anything>".

	int backend_major = _gl.VERSION[0] - '0';
	int backend_minor = _gl.VERSION[2] - '0';

	return backend_major > major || (backend_major == major && backend_minor >= minor);
}

} // namespace Yttrium
