#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/image.h>
#include <Yttrium/memory_manager.h>

#define BOOST_TEST_MODULE Image tests

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(tga_test)
{
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

BOOST_AUTO_TEST_CASE(png_test)
{
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

BOOST_AUTO_TEST_SUITE_END()
