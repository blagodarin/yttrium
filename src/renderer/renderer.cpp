#include "renderer.h"

#include <Yttrium/image.h>

#include "../terminal/window.h"

#include "gl/renderer.h"

namespace Yttrium
{

Renderer::Private::Private(Window *window, Allocator *allocator)
	: PrivateBase(allocator)
	, _window(*window)
{
	_builtin._renderer = this;

	_window._private->_renderer = this;
}

Renderer::Private::~Private()
{
	Y_ASSERT(_window._private->_renderer == this);

	_window._private->_renderer = nullptr;
}

void Renderer::Private::set_viewport(const Dim2 &size)
{
	_viewport_size = size;
	_screenshot_buffer.resize(size.width * size.height * 3);
}

void Renderer::Private::draw_rectangle(const Rectf &position, const Rectf &texture)
{
	size_t index = _vertices_2d.size();

	Vertex2D vertex;

	vertex.color = _color;

	vertex.position = position.top_left();
	vertex.texture = texture.top_left();
	_vertices_2d.push_back(vertex);

	vertex.position = position.outer_bottom_left();
	vertex.texture = texture.outer_bottom_left();
	_vertices_2d.push_back(vertex);

	vertex.position = position.outer_bottom_right();
	vertex.texture = texture.outer_bottom_right();
	_vertices_2d.push_back(vertex);

	vertex.position = position.outer_top_right();
	vertex.texture = texture.outer_top_right();
	_vertices_2d.push_back(vertex);

	_indices_2d.push_back(index + 0);
	_indices_2d.push_back(index + 1);
	_indices_2d.push_back(index + 2);
	_indices_2d.push_back(index + 3);
}

Renderer::Private *Renderer::Private::create(Window *window, Renderer::Backend backend, Allocator *allocator)
{
	Renderer::Private *result = nullptr;

	switch (backend)
	{
	case Renderer::OpenGl:

		result = new(allocator->allocate<OpenGlRenderer>())
			OpenGlRenderer(window, allocator);
		break;

	default:

		break;
	}

	if (!result)
	{
		Y_ABORT("Can't create renderer"); // NOTE: Safe to continue.
	}

	return result;
}

Renderer::Renderer(const Renderer &renderer)
	: _private(Private::copy(renderer._private))
{
}

Renderer::~Renderer()
{
	Private::release(&_private);
}

void Renderer::begin_frame()
{
	_private->clear();
}

void Renderer::end_frame()
{
	flush_2d();

	// TODO: Thread::sleep(0);

	_private->_window.swap_buffers();

	if (!_private->_screenshot_filename.is_empty() && _private->_screenshot_buffer.size())
	{
		_private->take_screenshot();

		ImageFormat format;

		format.depth = 1;
		format.channels = 3;
		format.pixel_format = PixelFormat::Bgr;
		format.orientation = ImageOrientation::XRightYUp;
		format.width = _private->_viewport_size.width;
		format.height = _private->_viewport_size.height;

		ImageWriter image(_private->_screenshot_filename, ImageType::Png);

		if (image.set_format(format))
		{
			image.write(_private->_screenshot_buffer.const_data());
		}
	}

	_private->_screenshot_filename.clear();
	_private->_builtin._is_bound = false;
}

void Renderer::flush_2d()
{
	if (_private->_vertices_2d.size())
	{
		_private->flush_2d();
		_private->_vertices_2d.clear();
		_private->_indices_2d.clear();
	}
}

RendererBuiltin Renderer::renderer_builtin()
{
	return RendererBuiltin(&_private->_builtin);
}

void Renderer::set_color(const Vector4f &color)
{
	_private->_color = color;
}

void Renderer::set_matrix_2d(double width, double height)
{
	if (!_private->_builtin._is_bound)
	{
		_private->set_matrix_2d(width, height);
		_private->_vertices_2d.clear();
		_private->_indices_2d.clear();
		_private->_rendering_size = Vector2d(width, height);
	}
}

void Renderer::set_matrix_2d_height(double height)
{
	set_matrix_2d(_private->_viewport_size.width * height / _private->_viewport_size.height, height);
}

void Renderer::set_matrix_2d_width(double width)
{
	set_matrix_2d(width, _private->_viewport_size.height * width / _private->_viewport_size.width);
}

Vector2d Renderer::rendering_size() const
{
	return _private->_rendering_size;
}

void Renderer::take_screenshot(const StaticString &name)
{
	_private->_screenshot_filename = name;
}

Dim2 Renderer::viewport_size() const
{
	return _private->_viewport_size;
}

Renderer &Renderer::operator =(const Renderer &renderer)
{
	Private::release(&_private);

	_private = Private::copy(renderer._private);

	return *this;
}

Renderer::Renderer(Window *window, Backend backend, Allocator *allocator)
	: _private(Private::create(window, backend, allocator))
{
}

Renderer::Renderer(Private *private_)
	: _private(Private::copy(private_))
{
}

} // namespace Yttrium
