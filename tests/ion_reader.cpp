#include <yttrium/ion/reader.h>
#include <yttrium/storage/source.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::IonReader;

namespace Yttrium
{
	inline bool operator==(const IonReader::Token& a, const IonReader::Token& b)
	{
		return a.line() == b.line() && a.column() == b.column() && a.type() == b.type() && a.text() == b.text();
	}

	inline std::ostream& operator<<(std::ostream& stream, const IonReader::Token& token)
	{
		return stream << "{" << token.line() << "," << token.column() << ",IonReader::Token::Type(" << static_cast<int>(token.type()) << "),R\"(" << token.text() << ")\"}"; // TODO: 'to_underlying_type'.
	}
}

BOOST_AUTO_TEST_CASE(test_ion_reader_iostream)
{
	std::ostringstream stream;
	stream << IonReader::Token{1, 2, IonReader::Token::Type::End, "test"};
	BOOST_CHECK_EQUAL(stream.str(), "{1,2,IonReader::Token::Type(" + std::to_string(static_cast<int>(IonReader::Token::Type::End)) + "),R\"(test)\"}");
}

BOOST_AUTO_TEST_CASE(test_ion_reader_token)
{
	using Yttrium::IonError;

	BOOST_TEST_CONTEXT("IonReader::Token::check_end")
	{
		BOOST_CHECK_NO_THROW(IonReader::Token(1, 1, IonReader::Token::Type::End, "").check_end());
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::ListEnd, "]").check_end(), IonError);
	}
	BOOST_TEST_CONTEXT("IonReader::Token::check_list_begin")
	{
		BOOST_CHECK_NO_THROW(IonReader::Token(1, 1, IonReader::Token::Type::ListBegin, "[").check_list_begin());
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::ObjectBegin, "{").check_list_begin(), IonError);
	}
	BOOST_TEST_CONTEXT("IonReader::Token::check_name")
	{
		BOOST_CHECK_NO_THROW(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").check_name("name1"));
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").check_name("name2"), IonError);
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::Value, "name1").check_name("name1"), IonError);
	}
	BOOST_TEST_CONTEXT("IonReader::Token::to_name")
	{
		BOOST_CHECK_EQUAL(IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1").to_name(), "name1");
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::Value, "name1").to_name(), IonError);
	}
	BOOST_TEST_CONTEXT("IonReader::Token::to_value")
	{
		BOOST_CHECK_EQUAL(IonReader::Token(1, 1, IonReader::Token::Type::Value, "value1").to_value(), "value1");
		BOOST_CHECK_THROW(IonReader::Token(1, 1, IonReader::Token::Type::Name, "value1").to_value(), IonError);
	}
}

namespace
{
	class TestData
	{
	public:
		TestData(const std::string& data) : _data(data) {}
		IonReader* operator->() { return &_ion_reader; }
	private:
		const std::string _data;
		const std::unique_ptr<const Yttrium::Source> _source{Yttrium::Source::from(_data.data(), _data.size())};
		IonReader _ion_reader{*_source};
	};
}

BOOST_AUTO_TEST_CASE(test_ion_reader_names)
{
	TestData ion{R"(name1)"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_values)
{
	TestData ion{R"(name1"value1""value2"name2)"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 14, IonReader::Token::Type::Value, "value2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 22, IonReader::Token::Type::Name, "name2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 27, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_lists)
{
	TestData ion{R"(name1["value1"[[][]]"value2"]name2)"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 7, IonReader::Token::Type::Value, "value1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 15, IonReader::Token::Type::ListBegin, "["));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 16, IonReader::Token::Type::ListBegin, "["));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 17, IonReader::Token::Type::ListEnd, "]"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 18, IonReader::Token::Type::ListBegin, "["));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 19, IonReader::Token::Type::ListEnd, "]"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 20, IonReader::Token::Type::ListEnd, "]"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 21, IonReader::Token::Type::Value, "value2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 29, IonReader::Token::Type::ListEnd, "]"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 30, IonReader::Token::Type::Name, "name2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 35, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_objects_and_names)
{
	TestData ion{R"(name1{name2}{name3{}}name4)"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 7, IonReader::Token::Type::Name, "name2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 12, IonReader::Token::Type::ObjectEnd, "}"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 13, IonReader::Token::Type::ObjectBegin, "{"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 14, IonReader::Token::Type::Name, "name3"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 19, IonReader::Token::Type::ObjectBegin, "{"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 20, IonReader::Token::Type::ObjectEnd, "}"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 21, IonReader::Token::Type::ObjectEnd, "}"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 22, IonReader::Token::Type::Name, "name4"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 27, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_spaces)
{
	TestData ion{" \t name1 \t \"value1\" \t [ \t \"value2\" \t ] \t { \t name2 \t } \t "};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 4, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 12, IonReader::Token::Type::Value, "value1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 23, IonReader::Token::Type::ListBegin, "["));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 27, IonReader::Token::Type::Value, "value2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 38, IonReader::Token::Type::ListEnd, "]"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 42, IonReader::Token::Type::ObjectBegin, "{"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 46, IonReader::Token::Type::Name, "name2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 54, IonReader::Token::Type::ObjectEnd, "}"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 58, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_newlines)
{
	TestData ion{"name1\nname2\r\nname3\rname4\n\r"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(2, 1, IonReader::Token::Type::Name, "name2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(3, 1, IonReader::Token::Type::Name, "name3"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(4, 1, IonReader::Token::Type::Name, "name4"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(6, 1, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_comments)
{
	TestData ion{"name1#name2\n\"value1\"\"val#ue2\"#\"value3\n#comment"};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(2, 1, IonReader::Token::Type::Value, "value1"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(2, 9, IonReader::Token::Type::Value, "val#ue2"));
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(3, 9, IonReader::Token::Type::End, ""));
}

BOOST_AUTO_TEST_CASE(test_ion_reader_negative)
{
	using Yttrium::IonError;

	{
		TestData ion{"\""};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"["};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"]"};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"{"};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"}"};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		using namespace std::literals::string_literals;
		TestData ion{"\0"s};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"\xff"};
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\0"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\n"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"\r"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1]"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1}"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1\"]"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1\"value1\"}"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::Value, "value1"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1[name2"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1[}"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ListBegin, "["));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1{\""};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1{["};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
	{
		TestData ion{"name1{]"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::ObjectBegin, "{"));
		BOOST_CHECK_THROW(ion->read(), IonError);
		BOOST_CHECK_THROW(ion->read(), IonError);
	}
}

BOOST_AUTO_TEST_CASE(test_ion_reader_empty_source)
{
	TestData ion{""};
	BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::End, ""));
}
