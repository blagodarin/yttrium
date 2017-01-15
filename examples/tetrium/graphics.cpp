#include "graphics.h"

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>

void TetriumGraphics::draw_field(const RectF& rect, const Tetrium::Field& field, const Tetrium::Figure& current_figure)
{
	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;
	const SizeF block_size(rect.width() / total_width, rect.height() / total_height);
	PushTexture push_texture(_renderer, _blocks.texture(), Texture2D::TrilinearFilter);
	draw_field_blocks(rect, block_size, field);
	draw_field_figure(rect, block_size, current_figure);
	draw_field_frame(rect, block_size);
}

void TetriumGraphics::draw_next_figure(const RectF& rect, const Tetrium::Figure& figure)
{
	if (figure.type() == Tetrium::Figure::None)
		return;
	PushTexture push_texture(_renderer, _blocks.texture(), Texture2D::TrilinearFilter);
	set_texture_rect(figure.type());
	const SizeF block_size(rect.width() / 4, rect.height() / 2);
	for (const auto& block : figure.blocks())
	{
		_renderer.draw_rect(
			{
				{
					rect.left() + block.x * block_size.width(),
					rect.top() + (1 - block.y / Tetrium::PointsPerRow) * block_size.height()
				},
				block_size
			});
	}
}

void TetriumGraphics::draw_field_blocks(const RectF& rect, const SizeF& block_size, const Tetrium::Field& field)
{
	for (int y = 0; y < Tetrium::Field::Height; ++y)
	{
		for (int x = 0; x < Tetrium::Field::Width; ++x)
		{
			const auto figure_type = field.blocks[y][x];
			if (figure_type != Tetrium::Figure::None)
			{
				set_texture_rect(figure_type);
				_renderer.draw_rect(
					{
						{
							rect.left() + (1 + x) * block_size.width(),
							rect.top() + (Tetrium::Field::Height - y) * block_size.height()
						},
						block_size
					});
			}
		}
	}
}

void TetriumGraphics::draw_field_figure(const RectF& rect, const SizeF& block_size, const Tetrium::Figure& figure)
{
	static const Vector2 frame_offset(1, Tetrium::Field::Height);

	if (figure.type() == Tetrium::Figure::None)
		return;

	set_texture_rect(figure.type());
	for (const auto& block : figure.blocks())
	{
		if (block.y < Tetrium::Field::Height * Tetrium::PointsPerRow)
		{
			_renderer.draw_rect(
				{
					{
						rect.left() + (frame_offset.x + block.x) * block_size.width(),
						rect.top() + (frame_offset.y - static_cast<float>(block.y) / Tetrium::PointsPerRow) * block_size.height()
					},
					block_size
				});
		}
	}
}

void TetriumGraphics::draw_field_frame(const RectF& rect, const SizeF& block_size)
{
	set_texture_rect(Tetrium::Figure::None);

	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;

	for (int i = 0; i < total_width; ++i)
		_renderer.draw_rect({{rect.left() + i * block_size.width(), rect.top()}, block_size});

	for (int i = 1; i < total_height - 1; ++i)
	{
		const auto y = rect.top() + i * block_size.height();
		_renderer.draw_rect({{rect.left(), y}, block_size});
		_renderer.draw_rect({{rect.left() + (total_width - 1) * block_size.width(), y}, block_size});
	}

	const auto bottom_y = rect.top() + (total_height - 1) * block_size.height();
	for (int i = 0; i < total_width; ++i)
		_renderer.draw_rect({{rect.left() + i * block_size.width(), bottom_y}, block_size});
}

void TetriumGraphics::set_texture_rect(Tetrium::Figure::Type figure_type)
{
	const int figure_index = (figure_type == Tetrium::Figure::None) ? 0 : figure_type + 1;
	_renderer.set_texture_rect({ _blocks.block_coords(figure_index), _blocks.block_size() }, {});
}
