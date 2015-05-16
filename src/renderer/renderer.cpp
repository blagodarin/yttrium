#include "renderer.h"

#include <yttrium/matrix.h>
#include "../memory/allocatable.h"
#include "debug_texture.h"
#include "gl/renderer.h"
#include "texture.h"

#include <cassert>

#if Y_IS_DEBUG
	#include <algorithm>
#endif

namespace Yttrium
{
	std::unique_ptr<RendererImpl> RendererImpl::create(WindowBackend& window, Allocator* allocator)
	{
		auto renderer = std::make_unique<GLRenderer>(window, allocator);
		if (!renderer->initialize())
			return {};
		ImageFormat debug_texture_format;
		debug_texture_format.set_width(DebugTexture::width);
		debug_texture_format.set_height(DebugTexture::height);
		debug_texture_format.set_orientation(ImageOrientation::XRightYDown);
		debug_texture_format.set_pixel_format(PixelFormat::Bgra, 32);
		renderer->_debug_texture = renderer->create_texture_2d(debug_texture_format, DebugTexture::data, false);
		if (!renderer->_debug_texture)
			return {};
		renderer->_debug_texture->set_filter(Texture2D::NearestFilter);
		return std::move(renderer);
	}

	RendererImpl::RendererImpl(Allocator* allocator)
		: _allocator(allocator)
		, _color(1, 1, 1)
		, _font_size(1, 1)
	{
		_texture_stack.emplace_back(Pointer<Texture2D>(), 1);
	}

	void RendererImpl::draw_rectangle(const RectF& rect)
	{
		draw_rectangle(rect, _texture_rect, current_texture_2d() ? _texture_borders : MarginsF());
	}

	void RendererImpl::draw_rectangle(const RectF& rect, const RectF& texture_rect)
	{
		draw_rectangle(rect, texture_rect, current_texture_2d() ? _texture_borders : MarginsF());
	}

	void RendererImpl::draw_text(const Vector2& position, const StaticString& text, unsigned alignment, TextCapture* capture)
	{
		if (!_font)
			return;

		Vector2 current_position = position;
		char last_symbol = '\0';
		float y_scaling = _font_size.y / _font.size();
		float x_scaling = y_scaling * _font_size.x;

		if (alignment != BottomRightAlignment)
		{
			const Vector2& size = text_size(text);

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

				const auto current_texture = current_texture_2d();
				Vector2 texture_top_left(current_texture->fix_coords(Vector2(info->rect.top_left())));
				Vector2 texture_bottom_right(current_texture->fix_coords(Vector2(info->rect.bottom_right())));

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

	void RendererImpl::set_color(const Vector4& color)
	{
		_color = color;
	}

	bool RendererImpl::set_font(const TextureFont& font)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture || (font && !Rect(current_texture->size()).contains(font.rect())))
			return false;

		_font = font;
		return true;
	}

	void RendererImpl::set_font_size(const Vector2& size)
	{
		_font_size = size;
	}

	bool RendererImpl::set_texture_borders(const Margins& borders)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return false;

		const Vector2 texture_size(current_texture->size());
		const Vector2& texture_rect_size = _texture_rect.size();
		const Vector2& min_size = Vector2(borders.min_size()) / texture_size;
		if (texture_rect_size.x < min_size.x || texture_rect_size.y < min_size.y)
			return false;

		_texture_borders = MarginsF(borders.top / texture_size.y, borders.right / texture_size.x,
			borders.bottom / texture_size.y, borders.right / texture_size.x);

		return true;
	}

	void RendererImpl::set_texture_rectangle(const RectF& rect)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return;

		const Vector2& top_left = current_texture->fix_coords(rect.top_left());
		const Vector2& bottom_right = current_texture->fix_coords(rect.bottom_right());

		_texture_rect.set_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
		_texture_borders = MarginsF();
	}

	Vector2 RendererImpl::text_size(const StaticString& text) const
	{
		return _font ? _font.text_size(text, _font_size) : Vector2(0, 0);
	}

	void RendererImpl::pop_projection()
	{
		flush_2d();
		assert(_matrix_stack.size() >= 2);
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Projection);
		_matrix_stack.pop_back();
		if (_matrix_stack.empty())
			return;
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		const auto last_projection = std::find_if(_matrix_stack.rbegin(), _matrix_stack.rend(),
			[](const std::pair<Matrix4, MatrixType>& matrix) { return matrix.second == MatrixType::Projection; });
		assert(last_projection != _matrix_stack.rend());
		set_projection(last_projection->first);
		set_transformation(_matrix_stack.back().first);
	}

	void RendererImpl::pop_texture()
	{
		assert(_texture_stack.size() > 1 || (_texture_stack.size() == 1 && _texture_stack.back().second > 1));
		if (_texture_stack.back().second > 1)
		{
			--_texture_stack.back().second;
			return;
		}
		flush_2d();
		_texture_stack.pop_back();
		update_current_texture();
	}

	void RendererImpl::pop_transformation()
	{
		flush_2d();
		assert(_matrix_stack.size() >= 3); // Projection, default transformation, transformation to pop.
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		set_transformation(_matrix_stack.back().first);
	}

	void RendererImpl::push_projection_2d(const Matrix4& matrix)
	{
		flush_2d();
		_matrix_stack.emplace_back(matrix, MatrixType::Projection);
		set_projection(_matrix_stack.back().first);
		_matrix_stack.emplace_back(Matrix4(), MatrixType::Transformation);
		set_transformation(_matrix_stack.back().first);
	}

	void RendererImpl::push_projection_3d(const Matrix4& matrix)
	{
		flush_2d();
		_matrix_stack.emplace_back(matrix, MatrixType::Projection);
		set_projection(_matrix_stack.back().first);
		// Make Y point forward and Z point up.
		_matrix_stack.emplace_back(Matrix4(
			1,  0,  0,  0,
			0,  0,  1,  0,
			0, -1,  0,  0,
			0,  0,  0,  1), MatrixType::Transformation);
		set_transformation(_matrix_stack.back().first);
	}

	void RendererImpl::push_texture(const Pointer<Texture2D>& texture)
	{
		assert(!_texture_stack.empty());
		if (_texture_stack.back().first == texture)
		{
			++_texture_stack.back().second;
			return;
		}
		flush_2d();
		_texture_stack.emplace_back(texture, 1);
		update_current_texture();
	}

	void RendererImpl::push_transformation(const Matrix4& matrix)
	{
		assert(_matrix_stack.size() >= 2 && _matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.emplace_back(_matrix_stack.back().first * matrix, MatrixType::Transformation);
		set_transformation(_matrix_stack.back().first);
	}

	RendererImpl::Statistics RendererImpl::reset_statistics()
	{
		const auto result = _statistics;
		_statistics = {};
		_seen_textures.clear();
		return result;
	}

	void RendererImpl::set_window_size(const Size& size)
	{
		_window_size = size;
		set_window_size_impl(_window_size);
	}

	BackendTexture2D* RendererImpl::current_texture_2d() const
	{
		return static_cast<BackendTexture2D*>(_texture_stack.back().first.get());
	}

	void RendererImpl::draw_rectangle(const RectF& position, const RectF& texture, const MarginsF& borders)
	{
		uint16_t index = _vertices_2d.size();

		if (index > 0)
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
			left_offset = borders.left * current_texture_2d()->size().width;

			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left;
			_vertices_2d.push_back(vertex);
		}

		if (borders.right > 0)
		{
			right_offset = borders.right * current_texture_2d()->size().width;

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
			float top_offset = borders.top * current_texture_2d()->size().height;

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
			float bottom_offset = borders.bottom * current_texture_2d()->size().height;

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

	void RendererImpl::flush_2d()
	{
		if (!_vertices_2d.empty())
		{
			flush_2d_impl(_vertices_2d, _indices_2d);
			_vertices_2d.clear();
			_indices_2d.clear();
		}
	}

	void RendererImpl::update_current_texture()
	{
		const auto* texture = current_texture_2d();
		// TODO: Lazy texture assignment (i.e. before actual rendering) to eliminate texture changes
		// when drawing differently textured geometry with another texture (e.g. GUI) on the stack.
		set_texture(texture);
		_texture_rect = texture ? texture->full_rectangle() : RectF();
		_texture_borders = MarginsF();
		_font = TextureFont();
		++_statistics._texture_switches;
#if Y_IS_DEBUG
		const auto i = std::find(_seen_textures.begin(), _seen_textures.end(), texture);
		if (i == _seen_textures.end())
			_seen_textures.emplace_back(texture);
		else
			++_statistics._redundant_texture_switches;
#endif
	}

	Push2D::Push2D(Renderer& renderer)
		: _renderer(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_projection_2d(Matrix4::projection_2d(_renderer.window_size()));
	}

	Push2D::~Push2D()
	{
		static_cast<RendererImpl&>(_renderer).pop_projection();
	}

	Push3D::Push3D(Renderer& renderer, const Matrix4& matrix)
		: _renderer(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_projection_3d(matrix);
	}

	Push3D::~Push3D()
	{
		static_cast<RendererImpl&>(_renderer).pop_projection();
	}

	PushTexture::PushTexture(Renderer& renderer, const Pointer<Texture2D>& texture)
		: _renderer(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_texture(texture);
	}

	PushTexture::~PushTexture()
	{
		static_cast<RendererImpl&>(_renderer).pop_texture();
	}

	PushTransformation::PushTransformation(Renderer& renderer, const Matrix4& matrix)
		: _renderer(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_transformation(matrix);
	}

	PushTransformation::~PushTransformation()
	{
		static_cast<RendererImpl&>(_renderer).pop_transformation();
	}
}
