#include "game.h"

#include "../utils.h"

namespace
{
	void make_checkerboard_texture(Storage& storage, const std::string& name)
	{
		storage.attach_buffer(name, ::make_bgra_tga(128, 128, [](size_t x, size_t y)
		{
			return ((x ^ y) & 1)
				? std::make_tuple(0xdd, 0xdd, 0xdd, 0xff)
				: std::make_tuple(0x00, 0x00, 0x00, 0xff);
		}));
	}
}

int main(int, char**)
{
	Storage storage{Storage::UseFileSystem::Before};
	::make_checkerboard_texture(storage, "examples/3d/data/checkerboard.tga");

	Game game{storage};
	game.run();
	return 0;
}
