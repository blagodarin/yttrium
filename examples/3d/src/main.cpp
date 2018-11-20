#include "game.h"

#include "../../utils.h"

namespace
{
	void make_checkerboard_texture(Storage& storage, const std::string& name)
	{
		storage.attach_buffer(name, ::make_bgra_tga(128, 128, [](size_t x, size_t y) {
			return ((x ^ y) & 1) ? Bgra32{ 0xdd, 0xdd, 0xdd } : Bgra32{ 0x00, 0x00, 0x00 };
		}));
	}
}

int main(int, char**)
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_package("3d.ypq");
	::make_checkerboard_texture(storage, "examples/3d/data/checkerboard.tga");

	Game game{ storage };
	game.run();
	return 0;
}
