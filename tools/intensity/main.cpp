#include <Yttrium/yttrium.h>

#include <cstdio>

using namespace Yttrium;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: intensity INPUT OUTPUT\n");
		return 1;
	}

	MemoryManager memory_manager;

	ImageReader input(argv[1]);

	if (!input.is_opened())
	{
		printf("ERROR: Invalid input file \"%s\"\n", argv[1]);
		return 1;
	}

	ImageFormat image_format = input.format();

	if (image_format.pixel_format() != PixelFormat::Gray || image_format.bits_per_pixel() != 8)
	{
		printf("ERROR: Input image \"%s\" is not grayscale 8-bit\n", argv[1]);
		return 1;
	}

	Buffer input_buffer(image_format.frame_size());

	if (!input.read(input_buffer.data()))
	{
		printf("ERROR: Unknown error while reading input file \"%s\"\n", argv[1]);
		return 1;
	}

	ImageWriter output(argv[2]);

	if (!output.is_opened())
	{
		printf("ERROR: Can't open output file \"%s\"\n", argv[2]);
		return 1;
	}

	image_format.set_pixel_format(PixelFormat::Bgra, 32);

	if (output.set_format(image_format))
	{
		printf("ERROR: Invalid input image \"%s\" format for output image \"%s\"\n", argv[1], argv[2]);
		return 1;
	}

	Buffer output_buffer(input_buffer.size() * 4);

	const uint8_t *src = static_cast<const uint8_t *>(input_buffer.const_data());
	uint8_t *dst = static_cast<uint8_t *>(output_buffer.data());

	for (size_t i = 0; i < input_buffer.size(); ++i)
	{
		*dst++ = *src;
		*dst++ = *src;
		*dst++ = *src;
		*dst++ = *src++;
	}

	if (!output.write(output_buffer.const_data()))
	{
		printf("ERROR: Unknown error while writing output file \"%s\"\n", argv[2]);
		return 1;
	}

	return 0;
}
