//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/memory/buffer.h>
#include <yttrium/ion/writer.h>
#include <yttrium/storage/writer.h>

#include <doctest.h>

using Yt::Buffer;
using Yt::IonWriter;
using Yt::Writer;

namespace
{
	class TestData
	{
	public:
		explicit TestData(IonWriter::Formatting formatting)
			: _ion_writer{ _writer, formatting } {}
		IonWriter* operator->() { return &_ion_writer; }
		std::string_view to_string() const { return { static_cast<const char*>(_buffer.data()), _buffer.size() }; }

	private:
		Buffer _buffer;
		Writer _writer{ _buffer };
		IonWriter _ion_writer;
	};
}

TEST_CASE("ion.writer.compact")
{
	TestData ion{ IonWriter::Formatting::Compact };

	SUBCASE("names")
	{
		ion->add_name("name1");
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1 name2");
	}

	SUBCASE("values")
	{
		ion->add_name("name1");
		ion->add_value("value1");
		ion->add_value("value2");
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1\"value1\"\"value2\"name2");
	}

	SUBCASE("lists")
	{
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
		CHECK(ion.to_string() == "name1[\"value1\"[[][]]\"value2\"]name2");
	}

	SUBCASE("objects_and_names")
	{
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
		CHECK(ion.to_string() == "name1{name2}{name3{}}name4");
	}

	SUBCASE("objects_and_values")
	{
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->add_value("value1");
		ion->end_object();
		ion->add_value("value2");
		ion->begin_object();
		ion->end_object();
		ion->flush();
		CHECK(ion.to_string() == "name1{name2\"value1\"}\"value2\"{}");
	}

	SUBCASE("objects_and_lists")
	{
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
		CHECK(ion.to_string() == "name1{name2[[{}]{}]}");
	}
}

TEST_CASE("ion.writer.escape")
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
	CHECK(ion.to_string() == "name"
							 "\"\\\"value\""
							 "\"value\\\"value\""
							 "\"value\\\"\""
							 "\"\\\\value\""
							 "\"value\\\\value\""
							 "\"value\\\\\"");
}

TEST_CASE("ion.writer.flush")
{
	SUBCASE("once.compact")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		CHECK(ion.to_string() == "name1");
	}

	SUBCASE("once.pretty")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		CHECK(ion.to_string() == "name1\n");
	}

	SUBCASE("twice.compact")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1 name2");
	}

	SUBCASE("twice.pretty")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1\n"
								 "name2\n");
	}

	SUBCASE("sequential.compact")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->add_name("name1");
		ion->flush();
		ion->flush();
		CHECK(ion.to_string() == "name1");
	}

	SUBCASE("sequential.pretty")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->add_name("name1");
		ion->flush();
		ion->flush();
		CHECK(ion.to_string() == "name1\n");
	}

	SUBCASE("nothing.compact")
	{
		TestData ion{ IonWriter::Formatting::Compact };
		ion->flush();
		CHECK(ion.to_string() == "");
	}

	SUBCASE("nothing.pretty")
	{
		TestData ion{ IonWriter::Formatting::Pretty };
		ion->flush();
		CHECK(ion.to_string() == "");
	}
}

TEST_CASE("ion.writer.negative")
{
	TestData ion{ IonWriter::Formatting::Compact };

	SUBCASE("Adding a name in a list")
	{
		ion->add_name("name1");
		ion->begin_list();
		CHECK_THROWS_AS(ion->add_name("name2"), std::logic_error);
	}

	SUBCASE("Adding a root object value without a name")
	{
		CHECK_THROWS_AS(ion->add_value("value1"), std::logic_error);
	}

	SUBCASE("Adding a value without a name")
	{
		ion->add_name("name1");
		ion->begin_object();
		CHECK_THROWS_AS(ion->add_value("value1"), std::logic_error);
	}

	SUBCASE("Beginning a list in a root object")
	{
		CHECK_THROWS_AS(ion->begin_list(), std::logic_error);
	}

	SUBCASE("Beginning a list in an object")
	{
		ion->add_name("name1");
		ion->begin_object();
		CHECK_THROWS_AS(ion->begin_list(), std::logic_error);
	}

	SUBCASE("Beginning an object in a root object")
	{
		CHECK_THROWS_AS(ion->begin_object(), std::logic_error);
	}

	SUBCASE("Beginning an object in an object")
	{
		ion->add_name("name1");
		ion->begin_object();
		CHECK_THROWS_AS(ion->begin_object(), std::logic_error);
	}

	SUBCASE("Ending a list as an object")
	{
		ion->add_name("name1");
		ion->begin_list();
		CHECK_THROWS_AS(ion->end_object(), std::logic_error);
	}

	SUBCASE("Ending an object as a list")
	{
		ion->add_name("name1");
		ion->begin_object();
		CHECK_THROWS_AS(ion->end_list(), std::logic_error);
	}

	SUBCASE("Ending a root object")
	{
		CHECK_THROWS_AS(ion->end_object(), std::logic_error);
	}

	SUBCASE("Flushing in a list")
	{
		ion->add_name("name1");
		ion->begin_list();
		CHECK_THROWS_AS(ion->flush(), std::logic_error);
	}

	SUBCASE("Flushing in a non-root object")
	{
		ion->add_name("name1");
		ion->begin_object();
		CHECK_THROWS_AS(ion->flush(), std::logic_error);
	}
}

TEST_CASE("ion.writer.pretty")
{
	TestData ion{ IonWriter::Formatting::Pretty };

	SUBCASE("names")
	{
		ion->add_name("name1");
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1\n"
								 "name2\n");
	}

	SUBCASE("values")
	{
		ion->add_name("name1");
		ion->add_value("value1");
		ion->add_value("value2");
		ion->add_name("name2");
		ion->flush();
		CHECK(ion.to_string() == "name1 \"value1\" \"value2\"\n"
								 "name2\n");
	}

	SUBCASE("lists")
	{
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
		CHECK(ion.to_string() == "name1 [\"value1\" [[] []] \"value2\"]\n"
								 "name2\n");
	}

	SUBCASE("objects_and_names")
	{
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
		CHECK(ion.to_string() == "name1\n"
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

	SUBCASE("objects_and_values")
	{
		ion->add_name("name1");
		ion->begin_object();
		ion->add_name("name2");
		ion->add_value("value1");
		ion->end_object();
		ion->add_value("value2");
		ion->begin_object();
		ion->end_object();
		ion->flush();
		CHECK(ion.to_string() == "name1\n"
								 "{\n"
								 "\tname2 \"value1\"\n"
								 "} \"value2\"\n"
								 "{\n"
								 "}\n");
	}

	SUBCASE("objects_and_lists")
	{
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
		CHECK(ion.to_string() == "name1\n"
								 "{\n"
								 "\tname2 [[\n"
								 "\t\t\t{\n"
								 "\t\t\t}]\n"
								 "\t\t{\n"
								 "\t\t}]\n"
								 "}\n");
	}
}
