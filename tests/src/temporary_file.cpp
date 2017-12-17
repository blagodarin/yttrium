#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>

#include <boost/test/unit_test.hpp>

using Yttrium::Reader;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Writer;

BOOST_AUTO_TEST_CASE(test_temporary_file)
{
	TemporaryFile file;
	BOOST_CHECK_EQUAL(0, Source::from(file)->size());
	{
		Writer writer{file};
		BOOST_CHECK_EQUAL(0, writer.size());
		writer.write(int64_t{-1});
		BOOST_CHECK_EQUAL(sizeof(int64_t), writer.size());
	}
	{
		const auto source = Source::from(file);
		BOOST_CHECK_EQUAL(sizeof(int64_t), source->size());
		Reader reader{*source};
		int64_t data = 0;
		BOOST_CHECK(reader.read(data));
		BOOST_CHECK_EQUAL(-1, data);
	}
	{
		Writer writer{file};
		BOOST_CHECK_EQUAL(0, writer.size());
	}
	BOOST_CHECK_EQUAL(0, Source::from(file)->size());
}
