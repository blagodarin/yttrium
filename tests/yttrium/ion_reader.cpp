#include <yttrium/ion/reader.h>
#include <yttrium/math/color.h>
#include <yttrium/storage/source.h>
#include <yttrium/utils/numeric.h>
#include "iostream.h"

#include <sstream>

#include <catch2/catch.hpp>

using Yttrium::IonError;
using Yttrium::IonReader;

namespace Yttrium
{
	inline bool operator==(const Color4f& a, const Color4f& b)
	{
		return a._r == b._r && a._g == b._g && a._b == b._b && a._a == b._a;
	}

	inline bool operator==(const IonReader::Token& a, const IonReader::Token& b)
	{
		return a.line() == b.line() && a.column() == b.column() && a.type() == b.type() && a.text() == b.text() && a.translatable() == b.translatable();
	}

	inline std::ostream& operator<<(std::ostream& stream, const IonReader::Token& token)
	{
		return stream << "{" << token.line() << "," << token.column() << ",IonReader::Token::Type(" << to_underlying(token.type()) << "),R\"(" << token.text() << ")\"}";
	}
}

TEST_CASE("ion.reader.token.check_end")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::End, "").check_end());
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::ListEnd, "]").check_end(), IonError);
}

TEST_CASE("ion.reader.token.check_list_begin")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::ListBegin, "[").check_list_begin());
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::ObjectBegin, "{").check_list_begin(), IonError);
}

TEST_CASE("ion.reader.token.check_list_end")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::ListEnd, "]").check_list_end());
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::ObjectEnd, "}").check_list_end(), IonError);
}

TEST_CASE("ion.reader.token.check_name")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").check_name("name1"));
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").check_name("name2"), IonError);
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::Value, "name1").check_name("name1"), IonError);
}

TEST_CASE("ion.reader.token.check_object_begin")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::ObjectBegin, "{").check_object_begin());
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::ListBegin, "[").check_object_begin(), IonError);
}

TEST_CASE("ion.reader.token.check_object_end")
{
	CHECK_NOTHROW(IonReader::Token(1, 1, IonReader::Token::Type::ObjectEnd, "}").check_object_end());
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::ListEnd, "]").check_object_end(), IonError);
}

TEST_CASE("ion.reader.token.iostream")
{
	std::ostringstream stream;
	stream << IonReader::Token{1, 2, IonReader::Token::Type::End, "test"};
	CHECK(stream.str() == "{1,2,IonReader::Token::Type(" + std::to_string(to_underlying(IonReader::Token::Type::End)) + "),R\"(test)\"}");
}

TEST_CASE("ion.reader.token.to_color")
{
	using Yttrium::Color4f;

	const auto color = [](std::string_view text)
	{
		return IonReader::Token{ 1, 1, IonReader::Token::Type::Color, text }.to_color();
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

	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::Value, "value").to_color(), IonError);
}

TEST_CASE("ion.reader.token.to_name")
{
	CHECK(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").to_name() == "name1");
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::Value, "name1").to_name(), IonError);
}

TEST_CASE("ion.reader.token.to_value")
{
	CHECK(IonReader::Token(1, 1, IonReader::Token::Type::Value, "value1").to_value() == "value1");
	CHECK_THROWS_AS(IonReader::Token(1, 1, IonReader::Token::Type::Name, "value1").to_value(), IonError);
}

namespace
{
	class TestData
	{
	public:
		explicit TestData(const std::string& data) : _data{data} {}
		IonReader* operator->() { return &_ion_reader; }
	private:
		const std::string _data;
		const std::unique_ptr<const Yttrium::Source> _source{Yttrium::Source::from(_data.data(), _data.size())};
		IonReader _ion_reader{*_source};
	};
}

TEST_CASE("ion.reader.names")
{
	TestData ion{R"(name1)"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.values")
{
	TestData ion{R"(name1"value1""value2"name2)"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
	CHECK(ion->read() == IonReader::Token(1, 14, IonReader::Token::Type::Value, "value2"));
	CHECK(ion->read() == IonReader::Token(1, 22, IonReader::Token::Type::Name, "name2"));
	CHECK(ion->read() == IonReader::Token(1, 27, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.lists")
{
	TestData ion{R"(name1["value1"[[][]]"value2"]name2)"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
	CHECK(ion->read() == IonReader::Token(1, 7, IonReader::Token::Type::Value, "value1"));
	CHECK(ion->read() == IonReader::Token(1, 15, IonReader::Token::Type::ListBegin, "["));
	CHECK(ion->read() == IonReader::Token(1, 16, IonReader::Token::Type::ListBegin, "["));
	CHECK(ion->read() == IonReader::Token(1, 17, IonReader::Token::Type::ListEnd, "]"));
	CHECK(ion->read() == IonReader::Token(1, 18, IonReader::Token::Type::ListBegin, "["));
	CHECK(ion->read() == IonReader::Token(1, 19, IonReader::Token::Type::ListEnd, "]"));
	CHECK(ion->read() == IonReader::Token(1, 20, IonReader::Token::Type::ListEnd, "]"));
	CHECK(ion->read() == IonReader::Token(1, 21, IonReader::Token::Type::Value, "value2"));
	CHECK(ion->read() == IonReader::Token(1, 29, IonReader::Token::Type::ListEnd, "]"));
	CHECK(ion->read() == IonReader::Token(1, 30, IonReader::Token::Type::Name, "name2"));
	CHECK(ion->read() == IonReader::Token(1, 35, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.objects_and_names")
{
	TestData ion{R"(name1{name2}{name3{}}name4)"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonReader::Token(1, 7, IonReader::Token::Type::Name, "name2"));
	CHECK(ion->read() == IonReader::Token(1, 12, IonReader::Token::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonReader::Token(1, 13, IonReader::Token::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonReader::Token(1, 14, IonReader::Token::Type::Name, "name3"));
	CHECK(ion->read() == IonReader::Token(1, 19, IonReader::Token::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonReader::Token(1, 20, IonReader::Token::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonReader::Token(1, 21, IonReader::Token::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonReader::Token(1, 22, IonReader::Token::Type::Name, "name4"));
	CHECK(ion->read() == IonReader::Token(1, 27, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.spaces")
{
	TestData ion{" \t name1 \t \"value1\" \t [ \t \"value2\" \t ] \t { \t name2 \t } \t "};
	CHECK(ion->read() == IonReader::Token(1, 4, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 12, IonReader::Token::Type::Value, "value1"));
	CHECK(ion->read() == IonReader::Token(1, 23, IonReader::Token::Type::ListBegin, "["));
	CHECK(ion->read() == IonReader::Token(1, 27, IonReader::Token::Type::Value, "value2"));
	CHECK(ion->read() == IonReader::Token(1, 38, IonReader::Token::Type::ListEnd, "]"));
	CHECK(ion->read() == IonReader::Token(1, 42, IonReader::Token::Type::ObjectBegin, "{"));
	CHECK(ion->read() == IonReader::Token(1, 46, IonReader::Token::Type::Name, "name2"));
	CHECK(ion->read() == IonReader::Token(1, 54, IonReader::Token::Type::ObjectEnd, "}"));
	CHECK(ion->read() == IonReader::Token(1, 58, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.newlines")
{
	TestData ion{"name1\nname2\r\nname3\rname4\n\r"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(2, 1, IonReader::Token::Type::Name, "name2"));
	CHECK(ion->read() == IonReader::Token(3, 1, IonReader::Token::Type::Name, "name3"));
	CHECK(ion->read() == IonReader::Token(4, 1, IonReader::Token::Type::Name, "name4"));
	CHECK(ion->read() == IonReader::Token(6, 1, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.comments.full_line")
{
	CHECK_THROWS_AS(TestData{"/comment"}->read(), IonError);
	CHECK_NOTHROW(TestData{"//comment"}->read());
	CHECK_NOTHROW(TestData{"///comment"}->read());
}

TEST_CASE("ion.reader.comments.inline")
{
	TestData ion{"name1//name2\n\"value1\"\"val//ue2\"//\"value3\n//comment"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(2, 1, IonReader::Token::Type::Value, "value1"));
	CHECK(ion->read() == IonReader::Token(2, 9, IonReader::Token::Type::Value, "val//ue2"));
	CHECK(ion->read() == IonReader::Token(3, 10, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.negatives")
{
	{
		TestData ion{"\""};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"["};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"]"};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"{"};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"}"};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		using namespace std::literals::string_literals;
		TestData ion{"\0"s};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"\xff"};
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\0"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\n"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\r"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1]"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1}"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1\"]"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1\"}"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1[name2"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1[}"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1{\""};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1{["};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
	{
		TestData ion{"name1{]"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		CHECK_THROWS_AS(ion->read(), IonError);
		CHECK_THROWS_AS(ion->read(), IonError);
	}
}

TEST_CASE("ion.reader.empty_source")
{
	TestData ion{""};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::End, ""));
}

TEST_CASE("ion.reader.translatable")
{
	TestData ion{R"(name1"value1"`value2`)"};
	CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	CHECK(ion->read() == IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
	CHECK(ion->read() == IonReader::Token(1, 14, IonReader::Token::Type::Value, "value2", true));
}

TEST_CASE("ion.reader.colors")
{
	{
		TestData ion{R"(name#01234567#89abcdef`)"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name"));
		CHECK(ion->read() == IonReader::Token(1, 5, IonReader::Token::Type::Color, "#01234567"));
		CHECK(ion->read() == IonReader::Token(1, 14, IonReader::Token::Type::Color, "#89abcdef"));
	}
	{
		TestData ion{R"(name#01234567#89ABCDEF`)"};
		CHECK(ion->read() == IonReader::Token(1, 1, IonReader::Token::Type::Name, "name"));
		CHECK(ion->read() == IonReader::Token(1, 5, IonReader::Token::Type::Color, "#01234567"));
		CHECK_THROWS_AS(ion->read(), IonError); // Only lowercase hexadecimal digits are allowed.
	}
}
