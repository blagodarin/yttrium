#include <yttrium/memory/buffer.h>
#include <yttrium/ion/writer.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/writer.h>

#include <boost/test/unit_test.hpp>

using Yttrium::Buffer;
using Yttrium::IonWriter;
using Yttrium::Writer;

namespace
{
	class TestData
	{
	public:
		TestData(IonWriter::Formatting formatting) : _ion_writer{ _writer, formatting } {}
		IonWriter* operator->() { return &_ion_writer; }
		std::string to_string() const { return { static_cast<const char*>(_buffer.data()), _buffer.size() }; }
	private:
		Buffer _buffer;
		Writer _writer{ _buffer };
		IonWriter _ion_writer;
	};
}

BOOST_AUTO_TEST_CASE(test_ion_writer_compact)
{
	BOOST_TEST_CONTEXT("Names")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1 name2");
	}
	BOOST_TEST_CONTEXT("Values")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->add_value("value1");
		ion->add_value("value2");
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1\"value1\"\"value2\"name2");
	}
	BOOST_TEST_CONTEXT("Lists")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_list();
		ion->add_value("value1");
		ion->begin_list();
		ion->begin_list();
		ion->end_list();
		ion->begin_list();
		ion->end_list();
		ion->end_list();
		ion->add_value("value2");
		ion->end_list();
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1[\"value1\"[[][]]\"value2\"]name2");
	}
	BOOST_TEST_CONTEXT("Objects and names")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->end_object();
		ion->begin_object();
		ion->add_name("name3");
		ion->begin_object();
		ion->end_object();
		ion->end_object();
		ion->add_name("name4");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1{name2}{name3{}}name4");
	}
	BOOST_TEST_CONTEXT("Objects and values")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->add_value("value1");
		ion->end_object();
		ion->add_value("value2");
		ion->begin_object();
		ion->end_object();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1{name2\"value1\"}\"value2\"{}");
	}
	BOOST_TEST_CONTEXT("Objects and lists")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->begin_list();
		ion->begin_list();
		ion->begin_object();
		ion->end_object();
		ion->end_list();
		ion->begin_object();
		ion->end_object();
		ion->end_list();
		ion->end_object();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1{name2[[{}]{}]}");
	}
}

BOOST_AUTO_TEST_CASE(test_ion_writer_escape)
{
	TestData ion{ IonWriter::Formatting::Compact };
	ion->add_name("name");
	ion->add_value("\"value");
	ion->add_value("value\"value");
	ion->add_value("value\"");
	ion->add_value("\\value");
	ion->add_value("value\\value");
	ion->add_value("value\\");
	ion->flush();
	const std::string expected{R"(name"\"value""value\"value""value\"""\\value""value\\value""value\\")"}; // Outside of BOOST_CHECK_EQUAL for Visual C++ 2017.
	BOOST_CHECK_EQUAL(ion.to_string(), expected);
}

BOOST_AUTO_TEST_CASE(test_ion_writer_flush)
{
	BOOST_TEST_CONTEXT("Flushing once (compact formatting)")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1");
	}
	BOOST_TEST_CONTEXT("Flushing once (pretty formatting)")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n");
	}
	BOOST_TEST_CONTEXT("Flushing twice (compact formatting)")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1 name2");
	}
	BOOST_TEST_CONTEXT("Flushing twice (pretty formatting)")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n"
			"name2\n");
	}
	BOOST_TEST_CONTEXT("Flushing sequentially (compact formatting)")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "name1");
	}
	BOOST_TEST_CONTEXT("Flushing sequentially (pretty formatting)")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n");
	}
	BOOST_TEST_CONTEXT("Flushing nothing (compact formatting)")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(), "");
	}
	BOOST_TEST_CONTEXT("Flushing nothing (pretty formatting)")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"");
	}
}

BOOST_AUTO_TEST_CASE(test_ion_writer_negative)
{
	BOOST_TEST_CONTEXT("Adding a name in a list")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_list();
		BOOST_CHECK_THROW(ion->add_name("name2"), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Adding a root object value without a name")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		BOOST_CHECK_THROW(ion->add_value("value1"), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Adding a value without a name")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		BOOST_CHECK_THROW(ion->add_value("value1"), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Beginning a list in a root object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		BOOST_CHECK_THROW(ion->begin_list(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Beginning a list in an object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		BOOST_CHECK_THROW(ion->begin_list(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Beginning an object in a root object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		BOOST_CHECK_THROW(ion->begin_object(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Beginning an object in an object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		BOOST_CHECK_THROW(ion->begin_object(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Ending a list as an object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_list();
		BOOST_CHECK_THROW(ion->end_object(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Ending an object as a list")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		BOOST_CHECK_THROW(ion->end_list(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Ending a root object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		BOOST_CHECK_THROW(ion->end_object(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Flushing in a list")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_list();
		BOOST_CHECK_THROW(ion->flush(), std::logic_error);
	}
	BOOST_TEST_CONTEXT("Flushing in a non-root object")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->begin_object();
		BOOST_CHECK_THROW(ion->flush(), std::logic_error);
	}
}

BOOST_AUTO_TEST_CASE(test_ion_writer_pretty)
{
	BOOST_TEST_CONTEXT("Names")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n"
			"name2\n");
	}
	BOOST_TEST_CONTEXT("Values")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->add_value("value1");
		ion->add_value("value2");
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1 \"value1\" \"value2\"\n"
			"name2\n");
	}
	BOOST_TEST_CONTEXT("Lists")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->begin_list();
		ion->add_value("value1");
		ion->begin_list();
		ion->begin_list();
		ion->end_list();
		ion->begin_list();
		ion->end_list();
		ion->end_list();
		ion->add_value("value2");
		ion->end_list();
		ion->add_name("name2");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1 [\"value1\" [[] []] \"value2\"]\n"
			"name2\n");
	}
	BOOST_TEST_CONTEXT("Objects and names")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->end_object();
		ion->begin_object();
		ion->add_name("name3");
		ion->begin_object();
		ion->end_object();
		ion->end_object();
		ion->add_name("name4");
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n"
			"{\n"
			"\tname2\n"
			"}\n"
			"{\n"
			"\tname3\n"
			"\t{\n"
			"\t}\n"
			"}\n"
			"name4\n");
	}
	BOOST_TEST_CONTEXT("Objects and values")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->add_value("value1");
		ion->end_object();
		ion->add_value("value2");
		ion->begin_object();
		ion->end_object();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n"
			"{\n"
			"\tname2 \"value1\"\n"
			"} \"value2\"\n"
			"{\n"
			"}\n");
	}
	BOOST_TEST_CONTEXT("Objects and lists")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->begin_list();
		ion->begin_list();
		ion->begin_object();
		ion->end_object();
		ion->end_list();
		ion->begin_object();
		ion->end_object();
		ion->end_list();
		ion->end_object();
		ion->flush();
		BOOST_CHECK_EQUAL(ion.to_string(),
			"name1\n"
			"{\n"
			"\tname2 [[\n"
			"\t\t\t{\n"
			"\t\t\t}]\n"
			"\t\t{\n"
			"\t\t}]\n"
			"}\n");
	}
}
