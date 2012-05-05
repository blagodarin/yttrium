#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

int main(int argc, char **argv)
{
	Application application;

	ImageReader reader("example.tga");

	ImageFormat format = reader.format();

	Buffer buffer(reader.format().frame_size());

	reader.read(buffer.data());
	reader.close();

	ImageWriter writer;

	writer.open("example.png");
	writer.set_format(format);
	writer.write(buffer.const_data());

	format.width /= 2;
	format.height *= 2;

	writer.open("example_squeezed.tga");
	writer.set_format(format);
	writer.write(buffer.const_data());

	return 0;
}
