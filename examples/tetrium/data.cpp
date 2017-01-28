#include "data.h"

#include <yttrium/image.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>

namespace
{
	void make_buttons_texture(Storage& storage, const StaticString& name)
	{
		constexpr auto button_size = 16u;
		constexpr auto button_styles = 4u;

		Image image({ button_size, button_size * button_styles, PixelFormat::Bgra, 32 });
		for (unsigned i = 0; i < button_styles; ++i)
		{
			for (unsigned y = 0; y < button_size; ++y)
			{
				for (unsigned x = 0; x < button_size; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + (i * button_size + y) * image.format().row_size() + x * 4;
					pixel[0] = 0xff;
					pixel[1] = 0x44 * i;
					pixel[2] = 0x44 * i;
					pixel[3] = 0xff;
				}
			}
		}

		storage.attach_buffer(name, image.to_buffer(ImageType::Tga));
	}
}

TetriumData::TetriumData(Storage& storage)
{
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture(storage, "examples/tetrium/data/buttons.tga");
}
