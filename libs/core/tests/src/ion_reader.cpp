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

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/math/color.h>
#include <yttrium/storage/source.h>
#include <yttrium/utils/numeric.h>
#include "iostream.h"

#include <sstream>

#include <catch2/catch.hpp>

using Yt::IonError;
using Yt::IonReader;
using Yt::IonToken;

namespace Yttrium
{
	inline bool operator==(const Color4f& a, const Color4f& b)
	{
		return a._r == b._r && a._g == b._g && a._b == b._b && a._a == b._a;
	}

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

TEST_CASE("ion.reader.token.to_color")
{
	using Yttrium::Color4f;

	const auto color = [](std::string_view text) {
		return IonToken{ 1, 1, IonToken::Type::ColorValue, text }.to_color();
	};

	CHECK(color("#00f") == Color4f(0.f, 0.f, 1.f, 1.f));
	CHECK(color("#f0f0") == Color4f(1.f, 0.f, 1.f, 0.f));
	CHECK(color("#0000ff") == Color4f(0.f, 0.f, 1.f, 1.f));
	CHECK(color("#ff00ff00") == Color4f(1.f, 0.f, 1.f, 0.f));

	CHECK_THROWS_AS(color("#"), IonError);
	CHECK_THROWS_AS(color("#1"), IonError);
	CHECK_THROWS_AS(color("#12"), IonError);
	CHECK_THROWS_AS(color("#12345"), IonError);
	CHECK_THROWS_AS(color("#1234567"), IonError);
	CHECK_THROWS_AS(color("#123456789"), IonError);

	CHECK_THROWS_AS(IonToken(1, 1, IonToken::Type::StringValue, "value").to_color(), IonError);
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
		const std::unique_ptr<const Yttrium::Source> _source{ Yttrium::Source::from(_data.data(), _data.size()) };
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

TEST_CASE("ion.reader.colors")
{
	{
		TestData ion{ R"(name#01234567#89abcdef`)" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name"));
		CHECK(ion->read() == IonToken(1, 5, IonToken::Type::ColorValue, "#01234567"));
		CHECK(ion->read() == IonToken(1, 14, IonToken::Type::ColorValue, "#89abcdef"));
	}
	{
		TestData ion{ R"(name#01234567#89ABCDEF`)" };
		CHECK(ion->read() == IonToken(1, 1, IonToken::Type::Name, "name"));
		CHECK(ion->read() == IonToken(1, 5, IonToken::Type::ColorValue, "#01234567"));
		CHECK_THROWS_AS(ion->read(), IonError); // Only lowercase hexadecimal digits are allowed.
	}
}
