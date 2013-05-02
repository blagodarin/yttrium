#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/image.h>
#include <Yttrium/memory_manager.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

// The TGA test must come first because the TGA format
// is the most basic one for both reading and writing.

BOOST_AUTO_TEST_CASE(tga_test)
{
	MemoryManager memory_manager;

	ImageReader reader;

	BOOST_REQUIRE(reader.open("tests/image/image.tga"));

	Buffer buffer1(reader.format().frame_size());

	BOOST_REQUIRE(reader.read(buffer1.data()));

	File file(File::Temporary);

	ImageWriter writer;

	BOOST_REQUIRE(writer.open(file.name(), ImageType::Tga));
	BOOST_REQUIRE(!writer.set_format(reader.format()));
	BOOST_REQUIRE(writer.write(buffer1.const_data()));

	writer.close();

	BOOST_REQUIRE(reader.open(file.name(), ImageType::Tga));

	Buffer buffer2(reader.format().frame_size());

	BOOST_REQUIRE(reader.read(buffer2.data()));
	BOOST_CHECK(buffer1 == buffer2);

	reader.close();
}

BOOST_AUTO_TEST_CASE(jpeg_test)
{
	MemoryManager memory_manager;

	ImageReader jpeg_reader;

	BOOST_REQUIRE(jpeg_reader.open("tests/image/jpeg.jpeg"));

	Buffer jpeg_buffer(jpeg_reader.format().frame_size());

	BOOST_REQUIRE(jpeg_reader.read(jpeg_buffer.data()));

	ImageReader tga_reader;

	BOOST_REQUIRE(tga_reader.open("tests/image/jpeg.tga"));
	BOOST_REQUIRE(tga_reader.set_format(jpeg_reader.format()));

	Buffer tga_buffer(tga_reader.format().frame_size());

	BOOST_REQUIRE(tga_reader.read(tga_buffer.data()));

	BOOST_CHECK(jpeg_buffer == tga_buffer);
}

BOOST_AUTO_TEST_CASE(png_test)
{
	MemoryManager memory_manager;

	ImageReader reader;

	BOOST_REQUIRE(reader.open("tests/image/image.tga"));

	Buffer buffer1(reader.format().frame_size());

	BOOST_REQUIRE(reader.read(buffer1.data()));

	File file(File::Temporary);

	ImageWriter writer;

	BOOST_REQUIRE(writer.open(file.name(), ImageType::Png));
	BOOST_REQUIRE(!writer.set_format(reader.format()));
	BOOST_REQUIRE(writer.write(buffer1.const_data()));

	writer.close();

	Buffer expected;
	Buffer actual;

	BOOST_REQUIRE(File("tests/image/image.png").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
