#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/image.h>
#include <yttrium/io/resource_manager.h>
#include <yttrium/log.h>

int main()
{
	Log::set_file("tetrium.log");

	NamedAllocator resources_allocator("resources");
	ResourceManager resource_manager(ResourceManager::UseFileSystem::Never, resources_allocator);
	if (!resource_manager.attach_package("tetrium.ypq"))
		return 1;

	NamedAllocator i18n_allocator("i18n");
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	{
		constexpr auto button_size = 16;
		constexpr auto button_styles = 4;

		ImageFormat format;
		format.set_width(button_size);
		format.set_height(button_size * button_styles);
		format.set_pixel_format(PixelFormat::Bgra, 32);
		format.set_orientation(ImageOrientation::XRightYDown);

		Image image(format);
		for (int i = 0; i < button_styles; ++i)
		{
			for (int y = 0; y < button_size; ++y)
			{
				for (int x = 0; x < button_size; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + (i * button_size + y) * format.row_size() + x * 4;
					pixel[0] = 0xff;
					pixel[1] = 0x44 * i;
					pixel[2] = 0x44 * i;
					pixel[3] = 0xff;
				}
			}
		}

		resource_manager.attach_buffer("examples/tetrium/data/blocks.tga", image.to_buffer(ImageType::Tga));
	}

	try
	{
		Game game(resource_manager);
		game.run();
		return 0;
	}
	catch (const std::runtime_error& e)
	{
		Log() << "Runtime error: " << e.what();
		return 1;
	}
}
