#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>

#include <catch/catch.hpp>

using Yttrium::Reader;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Writer;

TEST_CASE("temporary_file")
{
	TemporaryFile file;
	CHECK(Source::from(file)->size() == 0);
	{
		Writer writer{file};
		CHECK(writer.size() == 0);
		writer.write(int64_t{-1});
		CHECK(writer.size() == sizeof(int64_t));
	}
	{
		const auto source = Source::from(file);
		CHECK(source->size() == sizeof(int64_t));
		Reader reader{*source};
		int64_t data = 0;
		CHECK(reader.read(data));
		CHECK(data == -1);
	}
	{
		Writer writer{file};
		CHECK(writer.size() == 0);
	}
	CHECK(Source::from(file)->size() == 0);
}
