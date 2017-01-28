#include "game.h"

#include <yttrium/image.h>
#include <yttrium/log.h>
#include <yttrium/storage/storage.h>

namespace
{
	void make_buttons_texture(Storage& storage)
	{
		constexpr auto button_size = 16u;
		constexpr auto button_styles = 4u;

		Image image({ button_size, button_size * button_styles, PixelFormat::Bgra, 32 });
		for (int i = 0; i < button_styles; ++i)
		{
			for (int y = 0; y < button_size; ++y)
			{
				for (int x = 0; x < button_size; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + (i * button_size + y) * image.format().row_size() + x * 4;
					pixel[0] = 0xff;
					pixel[1] = 0x44 * i;
					pixel[2] = 0x44 * i;
					pixel[3] = 0xff;
				}
			}
		}

		storage.attach_buffer("examples/tetrium/data/buttons.tga", image.to_buffer(ImageType::Tga));
	}
}

int main()
{
	Log::set_file("tetrium.log");

	Storage storage(Storage::UseFileSystem::Never);
	if (!storage.attach_package("tetrium.ypq"))
		return 1;
	::make_buttons_texture(storage);

	try
	{
		Game game(storage);
		game.run();
		return 0;
	}
	catch (const std::runtime_error& e)
	{
		Log() << "Runtime error: " << e.what();
		return 1;
	}
}
