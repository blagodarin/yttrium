#include "renderer.h"

#include "../memory/allocatable.h"
#include "../terminal/window.h"
#include "gl/renderer.h"
#include "texture.h"

namespace Yttrium
{

Renderer::Private::Private(Window *window, Allocator *allocator)
	: PrivateBase(allocator)
	, _window(window)
	, _viewport_size(0)
	, _rendering_size(0)
	, _screenshot_filename(allocator)
	, _screenshot_image(allocator)
	, _color(1, 1, 1)
	, _font_size(1, 1)
{
	_builtin._renderer = this;

	_window->_renderer = this;

	ImageFormat screenshot_format;

	screenshot_format.set_pixel_format(PixelFormat::Rgb, 24);
	screenshot_format.set_orientation(ImageOrientation::XRightYUp);

	_screenshot_image.set_format(screenshot_format);
}

Renderer::Private::~Private()
{
	Y_ASSERT(_window->_renderer == this);
	_window->_renderer = nullptr;
}

void Renderer::Private::set_viewport(const Dim2 &size)
{
	_viewport_size = size;
}

void Renderer::Private::draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders)
{
	size_t index = _vertices_2d.size();

	if (index)
	{
		_indices_2d.push_back(index - 1);
		_indices_2d.push_back(index);

		if (_indices_2d.size() & 1)
		{
			_indices_2d.push_back(index); // Add an extra degenerate to ensure the correct face ordering.
		}
	}

	Vertex2D vertex;
	vertex.color = _color;

	float left_offset = 0;
	float right_offset = 0;

	// Outer top vertex row.

	vertex.position.y = position.top();
	vertex.texture.y = texture.top();

	vertex.position.x = position.left();
	vertex.texture.x = texture.left();
	_vertices_2d.push_back(vertex);

	if (borders.left > 0)
	{
		left_offset = borders.left * _texture->size().x * _rendering_size.x / _viewport_size.x;

		vertex.position.x = position.left() + left_offset;
		vertex.texture.x = texture.left() + borders.left;
		_vertices_2d.push_back(vertex);
	}

	if (borders.right > 0)
	{
		right_offset = borders.right * _texture->size().x * _rendering_size.x / _viewport_size.x;

		vertex.position.x = position.right() - right_offset;
		vertex.texture.x = texture.right() - borders.right;
		_vertices_2d.push_back(vertex);
	}

	vertex.position.x = position.right();
	vertex.texture.x = texture.right();
	_vertices_2d.push_back(vertex);

	// Top/only part indices.

	const size_t row_vertices = _vertices_2d.size() - index;

	for (size_t i = 0; i < row_vertices; ++i)
	{
		_indices_2d.push_back(index + i);
		_indices_2d.push_back(index + i + row_vertices);
	}

	if (borders.top > 0)
	{
		float top_offset = borders.top * _texture->size().y * _rendering_size.y / _viewport_size.y;

		// Inner top vertex row.

		vertex.position.y = position.top() + top_offset;
		vertex.texture.y = texture.top() + borders.top;

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		_vertices_2d.push_back(vertex);

		if (borders.left > 0)
		{
			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left;
			_vertices_2d.push_back(vertex);
		}

		if (borders.right > 0)
		{
			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders.right;
			_vertices_2d.push_back(vertex);
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		_vertices_2d.push_back(vertex);

		// Middle/bottom part indices.

		index += row_vertices;

		_indices_2d.push_back(index + row_vertices - 1);
		_indices_2d.push_back(index);

		for (size_t i = 0; i < row_vertices; ++i)
		{
			_indices_2d.push_back(index + i);
			_indices_2d.push_back(index + i + row_vertices);
		}
	}

	if (borders.bottom > 0)
	{
		float bottom_offset = borders.bottom * _texture->size().y * _rendering_size.y / _viewport_size.y;

		// Inner bottom vertex row.

		vertex.position.y = position.bottom() - bottom_offset;
		vertex.texture.y = texture.bottom() - borders.bottom;

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		_vertices_2d.push_back(vertex);

		if (borders.left > 0)
		{
			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left;
			_vertices_2d.push_back(vertex);
		}

		if (borders.right > 0)
		{
			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders.right;
			_vertices_2d.push_back(vertex);
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		_vertices_2d.push_back(vertex);

		// Bottom part indices.

		index += row_vertices;

		_indices_2d.push_back(index + row_vertices - 1);
		_indices_2d.push_back(index);

		for (size_t i = 0; i < row_vertices; ++i)
		{
			_indices_2d.push_back(index + i);
			_indices_2d.push_back(index + i + row_vertices);
		}
	}

	// Outer bottom vertex row.

	vertex.position.y = position.bottom();
	vertex.texture.y = texture.bottom();

	vertex.position.x = position.left();
	vertex.texture.x = texture.left();
	_vertices_2d.push_back(vertex);

	if (borders.left > 0)
	{
		vertex.position.x = position.left() + left_offset;
		vertex.texture.x = texture.left() + borders.left;
		_vertices_2d.push_back(vertex);
	}

	if (borders.right > 0)
	{
		vertex.position.x = position.right() - right_offset;
		vertex.texture.x = texture.right() - borders.right;
		_vertices_2d.push_back(vertex);
	}

	vertex.position.x = position.right();
	vertex.texture.x = texture.right();
	_vertices_2d.push_back(vertex);
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

			BackendTexture2D *backend_texture = static_cast<BackendTexture2D *>(_texture.pointer());
			Vector2f texture_top_left(backend_texture->fix_coords(info->area.top_left()));
			Vector2f texture_bottom_right(backend_texture->fix_coords(info->area.bottom_right()));

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

Renderer::Private *Renderer::Private::create(Window *window, Allocator *allocator)
{
	Allocatable<Renderer::Private> renderer(allocator);
	renderer.reset<OpenGlRenderer>(window);
	return renderer.release();
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

Allocator *Renderer::allocator() const
{
	return _private->_allocator;
}

void Renderer::begin_frame()
{
	_private->clear();
}

void Renderer::draw_rectangle(const RectF &rect)
{
	_private->draw_rectangle(rect, _private->_texture_rect,
		_private->_texture.is_null() ? MarginsF() : _private->_texture_borders);
}

void Renderer::draw_rectangle(const RectF &rect, const RectF &texture_rect)
{
	_private->draw_rectangle(rect, texture_rect,
		_private->_texture.is_null() ? MarginsF() : _private->_texture_borders);
}

void Renderer::draw_text(const Vector2f &position, const StaticString &text, Alignment alignment)
{
	_private->draw_text(position, text, alignment);
}

void Renderer::end_frame()
{
	flush_2d();

	_private->_window->swap_buffers();

	if (!_private->_screenshot_filename.is_empty())
	{
		_private->take_screenshot();
		_private->_screenshot_image.save(_private->_screenshot_filename, ImageType::Png);
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
	BackendTexture2D *backend_texture = static_cast<BackendTexture2D *>(_private->_texture.pointer());
	if (!backend_texture || (font && !Area(backend_texture->size()).contains(font.area())))
		return false;

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

void Renderer::set_texture(const Texture2DPtr &texture)
{
	if (_private->_builtin._is_bound || _private->_texture == texture)
	{
		return;
	}

	flush_2d();

	if (texture.is_null())
	{
		BackendTexture2D *old_backend_texture = static_cast<BackendTexture2D *>(_private->_texture.pointer());
		if (old_backend_texture)
			old_backend_texture->unbind();
	}

	_private->_texture = texture;
	_private->_texture_borders = MarginsF();

	BackendTexture2D *new_backend_texture = static_cast<BackendTexture2D *>(_private->_texture.pointer());
	if (new_backend_texture)
	{
		new_backend_texture->bind();
		_private->_texture_rect = new_backend_texture->full_rectangle();
	}
	else
	{
		_private->_texture_rect = RectF();
	}

	_private->_font = TextureFont();
}

bool Renderer::set_texture_borders(const MarginsI &borders)
{
	if (_private->_texture.is_null())
		return false;

	Vector2f&& texture_size = _private->_texture->size().to<float>();
	Vector2f&& texture_rect_size = _private->_texture_rect.size();
	Vector2f&& min_size = borders.min_size().to<float>() / texture_size;
	if (texture_rect_size.x < min_size.x || texture_rect_size.y < min_size.y)
		return false;

	_private->_texture_borders = MarginsF(borders.top / texture_size.y, borders.right / texture_size.x,
		borders.bottom / texture_size.y, borders.right / texture_size.x);

	return true;
}

void Renderer::set_texture_rectangle(const RectF &rect)
{
	BackendTexture2D *backend_texture = static_cast<BackendTexture2D *>(_private->_texture.pointer());
	if (backend_texture)
	{
		const Vector2f &top_left = backend_texture->fix_coords(rect.top_left());
		const Vector2f &bottom_right = backend_texture->fix_coords(rect.bottom_right());

		_private->_texture_rect.set_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
		_private->_texture_borders = MarginsF();
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

Dim2 Renderer::viewport_size() const
{
	return _private->_viewport_size;
}

Renderer &Renderer::operator =(const Renderer &renderer)
{
	Private::assign(&_private, renderer._private);

	return *this;
}

Renderer::Renderer(Window *window, Allocator *allocator)
	: _private(Private::create(window, allocator))
{
}

Renderer::Renderer(Private *private_)
	: _private(Private::copy(private_))
{
}

} // namespace Yttrium
