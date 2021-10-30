// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/exceptions.h>
#include <yttrium/base/numeric.h>
#include <yttrium/ion/reader.h>
#include <yttrium/storage/source.h>

#include <sstream>

#include <doctest/doctest.h>

using Yt::IonError;
using Yt::IonReader;
using Yt::IonToken;

namespace Yt
{
	inline bool operator==(const IonToken& a, const IonToken& b)
	{
		return a.line() == b.line() && a.column() == b.column() && a.type() == b.type() && a.text() == b.text() && a.translatable() == b.translatable();
	}

	inline std::ostream& operator<<(std::ostream& stream, const IonToken& token)
	{
		return stream << "{" << token.line() << "," << token.column() << ",IonToken::Type(" << to_underlying(token.type()) << "),R\"(" << token.text() << ")\"}";
	}
}

TEST_CASE("ion.reader.token.check_end")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::End, "").check_end());
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::ListEnd, "]").check_end(), IonError);
}

TEST_CASE("ion.reader.token.check_list_begin")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::ListBegin, "[").check_list_begin());
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::ObjectBegin, "{").check_list_begin(), IonError);
}

TEST_CASE("ion.reader.token.check_list_end")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::ListEnd, "]").check_list_end());
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::ObjectEnd, "}").check_list_end(), IonError);
}

TEST_CASE("ion.reader.token.check_name")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::Name, "name1").check_name("name1"));
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::Name, "name1").check_name("name2"), IonError);
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::StringValue, "name1").check_name("name1"), IonError);
}

TEST_CASE("ion.reader.token.check_object_begin")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::ObjectBegin, "{").check_object_begin());
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::ListBegin, "[").check_object_begin(), IonError);
}

TEST_CASE("ion.reader.token.check_object_end")
{
	CHECK_NOTHROW(IonToken(1, 1, IonToken::Type::ObjectEnd, "}").check_object_end());
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::ListEnd, "]").check_object_end(), IonError);
}

TEST_CASE("ion.reader.token.iostream")
{
	std::ostringstream stream;
	stream << IonToken{ 1, 2, IonToken::Type::End, "test" };
	CHECK(stream.str() == "{1,2,IonToken::Type(" + std::to_string(to_underlying(IonToken::Type::End)) + "),R\"(test)\"}");
}

TEST_CASE("ion.reader.token.to_name")
{
	CHECK(IonToken(1, 1, IonToken::Type::Name, "name1").to_name() == "name1");
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::StringValue, "name1").to_name(), IonError);
}

TEST_CASE("ion.reader.token.to_value")
{
	CHECK(IonToken(1, 1, IonToken::Type::StringValue, "value1").to_value() == "value1");
	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::Name, "value1").to_value(), IonError);
}

namespace
{
	class TestData
	{
	public:
		explicit TestData(const std::string& data)
			: _data{ data } {}
		IonReader* operator->() { return &_ion_reader; }

	private:
		const std::string _data;
		const std::unique_ptr<const Yt::Source> _source{ Yt::Source::from(_data.data(), _data.size()) };
		IonReader _ion_reader{ *_source };
	};
}

TEST_CASE("ion.reader.names")
{
	TestData ion{ R"(name1)" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 6, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.values")
{
	TestData ion{ R"(name1"value1""value2"name2)" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 6, IonToken::Type::StringValue, "value1"));
	CHECK(ion->read() == IonToken(1, 14, IonToken::Type::StringValue, "value2"));
	CHECK(ion->read() == IonToken(1, 22, IonToken::Type::Name, "name2"));
	CHECK(ion->read() == IonToken(1, 27, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.lists")
{
	TestData ion{ R"(name1["value1"[[][]]"value2"]name2)" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ListBegin, "["));
	CHECK(ion->read() == IonToken(1, 7, IonToken::Type::StringValue, "value1"));
	CHECK(ion->read() == IonToken(1, 15, IonToken::Type::ListBegin, "["));
	CHECK(ion->read() == IonToken(1, 16, IonToken::Type::ListBegin, "["));
	CHECK(ion->read() == IonToken(1, 17, IonToken::Type::ListEnd, "]"));
	CHECK(ion->read() == IonToken(1, 18, IonToken::Type::ListBegin, "["));
	CHECK(ion->read() == IonToken(1, 19, IonToken::Type::ListEnd, "]"));
	CHECK(ion->read() == IonToken(1, 20, IonToken::Type::ListEnd, "]"));
	CHECK(ion->read() == IonToken(1, 21, IonToken::Type::StringValue, "value2"));
	CHECK(ion->read() == IonToken(1, 29, IonToken::Type::ListEnd, "]"));
	CHECK(ion->read() == IonToken(1, 30, IonToken::Type::Name, "name2"));
	CHECK(ion->read() == IonToken(1, 35, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.objects_and_names")
{
	TestData ion{ R"(name1{name2}{name3{}}name4)" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonToken(1, 7, IonToken::Type::Name, "name2"));
	CHECK(ion->read() == IonToken(1, 12, IonToken::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonToken(1, 13, IonToken::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonToken(1, 14, IonToken::Type::Name, "name3"));
	CHECK(ion->read() == IonToken(1, 19, IonToken::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonToken(1, 20, IonToken::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonToken(1, 21, IonToken::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonToken(1, 22, IonToken::Type::Name, "name4"));
	CHECK(ion->read() == IonToken(1, 27, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.spaces")
{
	TestData ion{ " \t name1 \t \"value1\" \t [ \t \"value2\" \t ] \t { \t name2 \t } \t " };
	CHECK(ion->read() == IonToken(1, 4, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 12, IonToken::Type::StringValue, "value1"));
	CHECK(ion->read() == IonToken(1, 23, IonToken::Type::ListBegin, "["));
	CHECK(ion->read() == IonToken(1, 27, IonToken::Type::StringValue, "value2"));
	CHECK(ion->read() == IonToken(1, 38, IonToken::Type::ListEnd, "]"));
	CHECK(ion->read() == IonToken(1, 42, IonToken::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonToken(1, 46, IonToken::Type::Name, "name2"));
	CHECK(ion->read() == IonToken(1, 54, IonToken::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonToken(1, 58, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.newlines")
{
	TestData ion{ "name1\nname2\r\nname3\rname4\n\r" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(2, 1, IonToken::Type::Name, "name2"));
	CHECK(ion->read() == IonToken(3, 1, IonToken::Type::Name, "name3"));
	CHECK(ion->read() == IonToken(4, 1, IonToken::Type::Name, "name4"));
	CHECK(ion->read() == IonToken(6, 1, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.comments.full_line")
{
	CHECK_THROWS_AS(TestData("/comment")->read(), IonError);
	CHECK_NOTHROW(TestData("//comment")->read());
	CHECK_NOTHROW(TestData("///comment")->read());
}

TEST_CASE("ion.reader.comments.inline")
{
	TestData ion{ "name1//name2\n\"value1\"\"val//ue2\"//\"value3\n//comment" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(2, 1, IonToken::Type::StringValue, "value1"));
	CHECK(ion->read() == IonToken(2, 9, IonToken::Type::StringValue, "val//ue2"));
	CHECK(ion->read() == IonToken(3, 10, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.negatives")
{
	{
		TestData ion{ "\"" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "[" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "]" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "{" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "}" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		using namespace std::literals::string_literals;
		TestData ion{ "\0"s };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "\xff" };
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"value1" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"\0" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"\n" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"\r" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1]" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1}" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"value1\"]" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::StringValue, "value1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1\"value1\"}" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::StringValue, "value1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1[name2" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ListBegin, "["));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1[}" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ListBegin, "["));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1{\"" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1{[" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{ "name1{]" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
		CHECK(ion->read() == IonToken(1, 6, IonToken::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
}

TEST_CASE("ion.reader.empty_source")
{
	TestData ion{ "" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::End, ""));
}

TEST_CASE("ion.reader.translatable")
{
	TestData ion{ R"(name1"value1"`value2`)" };
	CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name1"));
	CHECK(ion->read() == IonToken(1, 6, IonToken::Type::StringValue, "value1"));
	CHECK(ion->read() == IonToken(1, 14, IonToken::Type::StringValue, "value2", true));
}
