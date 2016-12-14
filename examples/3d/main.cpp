#include "game.h"

#include <yttrium/image.h>
#include <yttrium/log.h>

namespace
{
	void make_checkerboard_texture(Storage& storage, const StaticString& name, size_t width, size_t height)
	{
		Image image({ width, height, PixelFormat::Bgra, 32 });
		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				const auto pixel = static_cast<uint8_t*>(image.data()) + y * image.format().row_size() + x * 4;
				const auto color = (x ^ y) & 1 ? uint8_t{ 0xdd } : uint8_t{ 0x00 };
				pixel[0] = color;
				pixel[1] = color;
				pixel[2] = color;
				pixel[3] = 0xff;
			}
		}
		storage.attach_buffer(name, image.to_buffer(ImageType::Tga));
	}
}

int main()
{
	Log::set_file("3d.log");

	Storage storage(Storage::UseFileSystem::Before);
	::make_checkerboard_texture(storage, "examples/3d/data/checkerboard.tga", 128, 128);

	Game game(storage);
	game.run();
	return 0;
}
