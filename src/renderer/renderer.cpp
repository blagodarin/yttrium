#include "renderer.h"

#include <Yttrium/image.h>

#include "../terminal/window.h"

#include "gl/renderer.h"

#include "texture.h"

namespace Yttrium
{

Renderer::Private::Private(Window *window, Allocator *allocator)
	: PrivateBase(allocator)
	, _window(*window)
	, _viewport_size(0)
	, _rendering_size(0)
	, _color(1, 1, 1)
	, _font_size(1, 1)
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
	_screenshot_buffer.resize(size.x * size.y * 3);
}

void Renderer::Private::draw_rectangle(const RectF &position, const RectF &texture)
{
	size_t index = _vertices_2d.size();

	Vertex2D vertex;

	vertex.color = _color;

	vertex.position = position.top_left();
	vertex.texture = texture.top_left();
	_vertices_2d.push_back(vertex);

	vertex.position = position.bottom_left();
	vertex.texture = texture.bottom_left();
	_vertices_2d.push_back(vertex);

	vertex.position = position.top_right();
	vertex.texture = texture.top_right();
	_vertices_2d.push_back(vertex);

	vertex.position = position.bottom_right();
	vertex.texture = texture.bottom_right();
	_vertices_2d.push_back(vertex);

	if (index)
	{
		_indices_2d.push_back(index - 1);
		_indices_2d.push_back(index);

		if (_indices_2d.size() & 1)
		{
			_indices_2d.push_back(index); // Add an extra degenerate to ensure the correct face ordering.
		}
	}

	_indices_2d.push_back(index);
	_indices_2d.push_back(index + 1);
	_indices_2d.push_back(index + 2);
	_indices_2d.push_back(index + 3);
}

void Renderer::Private::draw_text(const Vector2f &position, const StaticString &text, Alignment alignment)
{
	if (!_font)
	{
		return;
	}

	Vector2f current_position = position;
	char last_symbol = '\0';
	float y_scaling = _font_size.y / _font.size();
	float x_scaling = y_scaling * _font_size.x;

	if (alignment != BottomRightAlignment)
	{
		Vector2f size = text_size(text);

		if ((alignment & HorizontalAlignmentMask) != RightAlignment)
		{
			current_position.x -= size.x * (alignment & LeftAlignment ? 1.0 : 0.5);
		}

		if ((alignment & VerticalAlignmentMask) != BottomAlignment)
		{
			current_position.y -= size.y * (alignment & TopAlignment ? 1.0 : 0.5);
		}
	}

	const char *current_symbol = text.text();

	for (size_t i = 0; i < text.size(); ++i, ++current_symbol)
	{
		const TextureFont::CharInfo *info = _font.char_info(*current_symbol);

		if (info)
		{
			Vector2f symbol_position(
				current_position.x + x_scaling * info->offset.x,
				current_position.y + y_scaling * info->offset.y);

			Vector2f symbol_size(
				info->area.width() * x_scaling,
				info->area.height() * y_scaling);

			Vector2f texture_top_left(_texture._private->fix_coords(info->area.top_left()));
			Vector2f texture_bottom_right(_texture._private->fix_coords(info->area.bottom_right()));

			draw_rectangle(
				RectF(symbol_position, symbol_size),
				RectF::from_coords(
					texture_top_left.x, texture_top_left.y,
					texture_bottom_right.x, texture_bottom_right.y));

			current_position.x += x_scaling * (info->advance + _font.kerning(last_symbol, *current_symbol));
		}

		last_symbol = *current_symbol;
	}
}

Vector2f Renderer::Private::text_size(const StaticString &text) const
{
	return _font ? _font.text_size(text, _font_size) : Vector2f(0);
}

Renderer::Private *Renderer::Private::create(Window *window, Renderer::Backend backend, Allocator *allocator)
{
	Renderer::Private *result = nullptr;

	switch (backend)
	{
	case Renderer::OpenGl:

		result = Y_NEW(allocator, OpenGlRenderer)(window, allocator);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void Renderer::draw_rectangle(const RectF &rect)
{
	_private->draw_rectangle(rect, _private->_texture_rect);
}

void Renderer::draw_rectangle(const RectF &rect, const RectF &texture_rect)
{
	_private->draw_rectangle(rect, texture_rect);
}

void Renderer::draw_text(const Vector2f &position, const StaticString &text, Alignment alignment)
{
	_private->draw_text(position, text, alignment);
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

		format.set_pixel_format(PixelFormat::Bgr, 24);
		format.set_orientation(ImageOrientation::XRightYUp);
		format.set_width(_private->_viewport_size.x);
		format.set_height(_private->_viewport_size.y);

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

bool Renderer::set_font(const TextureFont &font)
{
	if (!_private->_texture || (font && !Area(_private->_texture.size()).contains(font.area())))
	{
		return false;
	}

	_private->_font = font;
	return true;
}

void Renderer::set_font_size(const Vector2f &size)
{
	_private->_font_size = size;
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
	set_matrix_2d(_private->_viewport_size.x * height / _private->_viewport_size.y, height);
}

void Renderer::set_matrix_2d_width(double width)
{
	set_matrix_2d(width, _private->_viewport_size.y * width / _private->_viewport_size.x);
}

void Renderer::set_texture(const Texture2D &texture)
{
	if (_private->_builtin._is_bound || _private->_texture == texture)
	{
		return;
	}

	flush_2d();

	if (_private->_texture && !texture)
	{
		_private->_texture._private->unbind();
	}

	_private->_texture = texture;

	if (_private->_texture)
	{
		_private->_texture._private->bind();
		_private->_texture_rect = _private->_texture._private->full_rectangle();
	}

	_private->_font = TextureFont();
}

void Renderer::set_texture_rectangle(const RectF &rect)
{
	if (_private->_texture)
	{
		const Vector2f &top_left = _private->_texture._private->fix_coords(rect.top_left());
		const Vector2f &bottom_right = _private->_texture._private->fix_coords(rect.bottom_right());

		_private->_texture_rect.set_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
	}
}

Vector2d Renderer::rendering_size() const
{
	return _private->_rendering_size;
}

void Renderer::take_screenshot(const StaticString &name)
{
	_private->_screenshot_filename = name;
}

Vector2f Renderer::text_size(const StaticString &text) const
{
	return _private->text_size(text);
}

TextureCache Renderer::texture_cache()
{
	return TextureCache(_private->texture_cache());
}

Dim2 Renderer::viewport_size() const
{
	return _private->_viewport_size;
}

Renderer &Renderer::operator =(const Renderer &renderer)
{
	Private::assign(&_private, renderer._private);

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
