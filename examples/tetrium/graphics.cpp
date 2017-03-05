#include "graphics.h"

#include <yttrium/image.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>

namespace
{
	struct Rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		Rgb(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
	};

	const std::array<int, 24> weights =
	{
		2, 2, 2, // Gray.
		1, 0, 0, // Red.
		1, 2, 0, // Orange.
		1, 1, 0, // Yellow.
		0, 1, 0, // Green.
		0, 1, 1, // Light blue.
		0, 0, 1, // Blue.
		2, 0, 1, // Purple.
	};

	enum { TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight };

	const std::array<int, 9> pattern =
	{
		0, 0, 3,
		1, 2, 3,
		1, 4, 4,
	};

	static constexpr auto FragmentSize = 32;
	static constexpr auto FragmentCount = 8;
	static constexpr auto Border = 3;

	const SizeF BlockSize(FragmentSize - 2, FragmentSize - 2);

	Rgb pixel_color(size_t block, size_t x, size_t y)
	{
		const auto pixel_type = [](auto x_, auto y_)
		{
			const auto rx = FragmentSize - 1 - x_;
			if (y_ < Border)
			{
				if (x_ < Border)
					return x_ > y_ ? Top : x_ < y_ ? Left : TopLeft;
				else if (rx < Border && y_ < Border)
					return rx > y_ ? Top : rx < y_ ? Right : TopRight;
				else
					return Top;
			}
			const auto ry = FragmentSize - 1 - y_;
			if (ry < Border)
			{
				if (x_ < Border)
					return x_ > ry ? Bottom : x_ < ry ? Left : BottomLeft;
				else if (rx < Border && ry < Border)
					return rx > ry ? Bottom : rx < ry ? Right : BottomRight;
				else
					return Bottom;
			}
			return x_ < Border ? Left : rx < Border ? Right : Center;
		};
		const auto base = 255;
		const auto offset = 20 * pattern[pixel_type(x, y)];
		const auto scale = [block, base, offset](size_t index)
		{
			const auto weight = weights[block * 3 + index];
			return weight ? base / weight - offset : 0;
		};
		return Rgb(scale(0), scale(1), scale(2));
	}

	Image make_blocks_image()
	{
		Image image({ FragmentSize, FragmentSize * FragmentCount, PixelFormat::Bgra, 32 });
		for (size_t i = 0; i < FragmentCount; ++i)
		{
			for (size_t y = 0; y < FragmentSize; ++y)
			{
				for (size_t x = 0; x < FragmentSize; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + (i * FragmentSize + y) * image.format().row_size() + x * 4;
					const auto color = ::pixel_color(i, x, y);
					pixel[0] = color.b;
					pixel[1] = color.g;
					pixel[2] = color.r;
					pixel[3] = 0xff;
				}
			}
		}
		return image;
	}

	RectF block_rect(size_t index)
	{
		return { PointF(1, index * FragmentSize + 1), SizeF(FragmentSize - 2, FragmentSize - 2) };
	}
}

TetriumGraphics::TetriumGraphics(Renderer& renderer)
	: _renderer(renderer)
	, _blocks_texture(_renderer.create_texture_2d(::make_blocks_image()))
{
}

void TetriumGraphics::draw(const std::string& canvas, const RectF& rect, const Tetrium::Game& logic) const
{
	if (canvas == "field")
		draw_field(rect, logic.field(), logic.current_figure());
	else if (canvas == "next")
		draw_next_figure(rect, logic.next_figure());
}

void TetriumGraphics::draw_field(const RectF& rect, const Tetrium::Field& field, const Tetrium::Figure& current_figure) const
{
	static const int total_width = 1 + Tetrium::Field::Width + 1;
	static const int total_height = 1 + Tetrium::Field::Height + 1;
	const SizeF block_size(rect.width() / total_width, rect.height() / total_height);
	PushTexture push_texture(_renderer, _blocks_texture.get(), Texture2D::TrilinearFilter);
	draw_field_blocks(rect, block_size, field);
	draw_field_figure(rect, block_size, current_figure);
	draw_field_frame(rect, block_size);
}

void TetriumGraphics::draw_next_figure(const RectF& rect, const Tetrium::Figure& figure) const
{
	if (figure.type() == Tetrium::Figure::None)
		return;
	PushTexture push_texture(_renderer, _blocks_texture.get(), Texture2D::TrilinearFilter);
	set_texture_rect(figure.type());
	const SizeF block_size(rect.width() / 4, rect.height() / 2);
	for (const auto& block : figure.blocks())
		draw_block(rect, block_size, block.x, 1 - block.y / Tetrium::PointsPerRow);
}

void TetriumGraphics::draw_block(const RectF& rect, const SizeF& block_size, float x, float y) const
{
	_renderer.draw_rect({ { rect.left() + x * block_size.width(), rect.top() + y * block_size.height() }, block_size });
}

void TetriumGraphics::draw_field_blocks(const RectF& rect, const SizeF& block_size, const Tetrium::Field& field) const
{
	for (int y = 0; y < Tetrium::Field::Height; ++y)
	{
		for (int x = 0; x < Tetrium::Field::Width; ++x)
		{
			const auto figure_type = field.blocks[y][x];
			if (figure_type == Tetrium::Figure::None)
				continue;
			set_texture_rect(figure_type);
			draw_block(rect, block_size, 1 + x, Tetrium::Field::Height - y);
		}
	}
}

void TetriumGraphics::draw_field_figure(const RectF& rect, const SizeF& block_size, const Tetrium::Figure& figure) const
{
	static const Vector2 frame_offset(1, Tetrium::Field::Height);
	if (figure.type() == Tetrium::Figure::None)
		return;
	set_texture_rect(figure.type());
	for (const auto& block : figure.blocks())
		if (block.y < Tetrium::Field::Height * Tetrium::PointsPerRow)
			draw_block(rect, block_size, frame_offset.x + block.x, frame_offset.y - static_cast<float>(block.y) / Tetrium::PointsPerRow);
}

void TetriumGraphics::draw_field_frame(const RectF& rect, const SizeF& block_size) const
{
	static const int total_width = 1 + Tetrium::Field::Width + 1;
	static const int total_height = 1 + Tetrium::Field::Height + 1;
	set_texture_rect(Tetrium::Figure::None);
	for (int i = 0; i < total_width; ++i)
		draw_block(rect, block_size, i, 0);
	for (int i = 1; i < total_height - 1; ++i)
	{
		draw_block(rect, block_size, 0, i);
		draw_block(rect, block_size, total_width - 1, i);
	}
	for (int i = 0; i < total_width; ++i)
		draw_block(rect, block_size, i, total_height - 1);
}

void TetriumGraphics::set_texture_rect(Tetrium::Figure::Type figure_type) const
{
	const auto figure_index = (figure_type == Tetrium::Figure::None) ? 0 : static_cast<size_t>(figure_type) + 1;
	_renderer.set_texture_rect(::block_rect(figure_index), {});
}
