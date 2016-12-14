#include "blocks.h"

#include <yttrium/image.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>

#include <array>

namespace
{
	struct Rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		Rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
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

	static constexpr int FragmentSize = 32;
	static constexpr int FragmentCount = 8;
	static constexpr int Border = 3;

	Rgb pixel_color(int block, int x, int y)
	{
		const auto pixel_type = [](int x, int y)
		{
			const int rx = FragmentSize - 1 - x;
			if (y < Border)
			{
				if (x < Border)
					return x > y ? Top : x < y ? Left : TopLeft;
				else if (rx < Border && y < Border)
					return rx > y ? Top : rx < y ? Right : TopRight;
				else
					return Top;
			}
			const int ry = FragmentSize - 1 - y;
			if (ry < Border)
			{
				if (x < Border)
					return x > ry ? Bottom : x < ry ? Left : BottomLeft;
				else if (rx < Border && ry < Border)
					return rx > ry ? Bottom : rx < ry ? Right : BottomRight;
				else
					return Bottom;
			}
			return x < Border ? Left : rx < Border ? Right : Center;
		};
		const auto base = 255;
		const auto offset = 20 * pattern[pixel_type(x, y)];
		const auto scale = [block, base, offset](int index)
		{
			const auto weight = weights[block * 3 + index];
			return weight ? base / weight - offset : 0;
		};
		return Rgb(scale(0), scale(1), scale( 2));
	}
}

Blocks::Blocks(Renderer& renderer)
{
	Image image({ FragmentSize, FragmentSize * FragmentCount, PixelFormat::Bgra, 32 });
	for (int i = 0; i < FragmentCount; ++i)
	{
		for (int y = 0; y < FragmentSize; ++y)
		{
			for (int x = 0; x < FragmentSize; ++x)
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

	_texture = renderer.create_texture_2d(image);
}

Blocks::~Blocks() = default;

SizeF Blocks::block_size()
{
	return SizeF(FragmentSize - 2, FragmentSize - 2);
}

PointF Blocks::block_coords(size_t index)
{
	return PointF(1, index * FragmentSize + 1);
}
