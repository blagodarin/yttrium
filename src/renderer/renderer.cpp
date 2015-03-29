#include "renderer.h"

#include "../memory/allocatable.h"
#include "gl/renderer.h"
#include "texture.h"

#include <cassert>

namespace Yttrium
{
	std::unique_ptr<RendererImpl> RendererImpl::create(WindowBackend& window, Allocator* allocator)
	{
		auto renderer = std::make_unique<OpenGlRenderer>(window, allocator);
		if (!renderer->initialize())
			return {};
		return std::move(renderer);
	}

	RendererImpl::RendererImpl(WindowBackend& window, Allocator* allocator)
		: _allocator(allocator)
		, _window(window)
		, _color(1, 1, 1)
		, _font_size(1, 1)
	{
	}

	void RendererImpl::draw_rectangle(const RectF& rect)
	{
		draw_rectangle(rect, _texture_rect, _texture ? _texture_borders : MarginsF());
	}

	void RendererImpl::draw_rectangle(const RectF& rect, const RectF& texture_rect)
	{
		draw_rectangle(rect, texture_rect, _texture ? _texture_borders : MarginsF());
	}

	void RendererImpl::draw_text(const Vector2f& position, const StaticString& text, unsigned alignment, TextCapture* capture)
	{
		if (!_font)
			return;

		Vector2f current_position = position;
		char last_symbol = '\0';
		float y_scaling = _font_size.y / _font.size();
		float x_scaling = y_scaling * _font_size.x;

		if (alignment != BottomRightAlignment)
		{
			const Vector2f& size = text_size(text);

			if ((alignment & HorizontalAlignmentMask) != RightAlignment)
				current_position.x -= size.x * (alignment & LeftAlignment ? 1.0 : 0.5);

			if ((alignment & VerticalAlignmentMask) != BottomAlignment)
				current_position.y -= size.y * (alignment & TopAlignment ? 1.0 : 0.5);
		}

		float selection_left = 0.f;
		const auto do_capture = [&](unsigned index)
		{
			if (!capture)
				return;

			if (capture->cursor_pos == index)
			{
				capture->cursor_rect = RectF(current_position.x, current_position.y, 2, _font_size.y);
				capture->has_cursor = true;
			}

			if (capture->selection_begin < capture->selection_end)
			{
				if (index == capture->selection_begin)
				{
					selection_left = current_position.x;
				}
				else if (index == capture->selection_end)
				{
					capture->selection_rect.set_coords(selection_left, current_position.y,
						current_position.x, current_position.y + _font_size.y);
					capture->has_selection = true;
				}
			}
		};

		const char* current_symbol = text.text();

		for (size_t i = 0; i < text.size(); ++i, ++current_symbol)
		{
			const TextureFont::CharInfo* info = _font.char_info(*current_symbol);

			if (info)
			{
				const RectF symbol_rect(
					current_position.x + x_scaling * info->offset.x,
					current_position.y + y_scaling * info->offset.y,
					info->rect.width() * x_scaling,
					info->rect.height() * y_scaling);

				BackendTexture2D* backend_texture = static_cast<BackendTexture2D*>(_texture.get());
				Vector2f texture_top_left(backend_texture->fix_coords(info->rect.top_left()));
				Vector2f texture_bottom_right(backend_texture->fix_coords(info->rect.bottom_right()));

				draw_rectangle(symbol_rect,
					RectF::from_coords(texture_top_left.x, texture_top_left.y, texture_bottom_right.x, texture_bottom_right.y),
					MarginsF());

				do_capture(i);

				current_position.x += x_scaling * (info->advance + _font.kerning(last_symbol, *current_symbol));
			}

			last_symbol = *current_symbol;
		}

		do_capture(text.size());
	}

	void RendererImpl::set_color(const Vector4f& color)
	{
		_color = color;
	}

	bool RendererImpl::set_font(const TextureFont& font)
	{
		BackendTexture2D* backend_texture = static_cast<BackendTexture2D*>(_texture.get());
		if (!backend_texture || (font && !Rect(backend_texture->size()).contains(font.rect())))
			return false;

		_font = font;
		return true;
	}

	void RendererImpl::set_font_size(const Vector2f& size)
	{
		_font_size = size;
	}

	void RendererImpl::set_texture(const Pointer<Texture2D>& texture)
	{
		if (_texture == texture)
			return;

		flush_2d();

		if (!texture)
		{
			BackendTexture2D* old_backend_texture = static_cast<BackendTexture2D*>(_texture.get());
			if (old_backend_texture)
				old_backend_texture->unbind();
		}

		_texture = texture;
		_texture_borders = MarginsF();

		BackendTexture2D* new_backend_texture = static_cast<BackendTexture2D*>(_texture.get());
		if (new_backend_texture)
		{
			new_backend_texture->bind();
			_texture_rect = new_backend_texture->full_rectangle();
		}
		else
		{
			_texture_rect = RectF();
		}

		_font = TextureFont();
	}

	bool RendererImpl::set_texture_borders(const MarginsI& borders)
	{
		if (!_texture)
			return false;

		const Vector2f texture_size(_texture->size());
		const Vector2f& texture_rect_size = _texture_rect.size();
		const Vector2f& min_size = borders.min_size().to<float>() / texture_size;
		if (texture_rect_size.x < min_size.x || texture_rect_size.y < min_size.y)
			return false;

		_texture_borders = MarginsF(borders.top / texture_size.y, borders.right / texture_size.x,
			borders.bottom / texture_size.y, borders.right / texture_size.x);

		return true;
	}

	void RendererImpl::set_texture_rectangle(const RectF& rect)
	{
		BackendTexture2D* backend_texture = static_cast<BackendTexture2D*>(_texture.get());
		if (backend_texture)
		{
			const Vector2f& top_left = backend_texture->fix_coords(rect.top_left());
			const Vector2f& bottom_right = backend_texture->fix_coords(rect.bottom_right());

			_texture_rect.set_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
			_texture_borders = MarginsF();
		}
	}

	Vector2f RendererImpl::text_size(const StaticString& text) const
	{
		return _font ? _font.text_size(text, _font_size) : Vector2f(0);
	}

	Size RendererImpl::window_size() const
	{
		return _window_size;
	}

	void RendererImpl::flush_2d()
	{
		if (!_vertices_2d.empty())
		{
			flush_2d_impl();
			_vertices_2d.clear();
			_indices_2d.clear();
		}
	}

	void RendererImpl::set_debug_rendering(bool enable)
	{
		assert(_debug_rendering != enable);
		if (enable)
		{
			flush_2d();
			_texture = {}; // No need to unbind it, we're going to bind the debug texture right now.
			_texture_rect = RectF();
			_texture_borders = {};
			_font = {};
			bind_debug_texture();
		}
		else
		{
			flush_2d();
			// TODO: unbind_debug_texture(); // It's better to finish with no texture bound.
		}
		_debug_rendering = enable;
	}

	void RendererImpl::set_window_size(const Size& size)
	{
		_window_size = size;
		update_window_size();
	}

	void RendererImpl::draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders)
	{
		size_t index = _vertices_2d.size();

		if (index)
		{
			_indices_2d.push_back(index - 1);
			_indices_2d.push_back(index);

			if (_indices_2d.size() & 1)
				_indices_2d.push_back(index); // Add an extra degenerate to ensure the correct face ordering.
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
			left_offset = borders.left * _texture->size().width;

			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left;
			_vertices_2d.push_back(vertex);
		}

		if (borders.right > 0)
		{
			right_offset = borders.right * _texture->size().width;

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
			float top_offset = borders.top * _texture->size().height;

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
			float bottom_offset = borders.bottom * _texture->size().height;

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
}
