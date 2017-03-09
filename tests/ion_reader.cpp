#include <yttrium/ion/reader.h>
#include <yttrium/storage/reader.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::IonReader;
using Yttrium::Reader;

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

namespace
{
	class TestData
	{
	public:
		TestData(const std::string& data) : _ion_reader{Reader{data.data(), data.size()}} {}
		IonReader* operator->() { return &_ion_reader; }
	private:
		IonReader _ion_reader;
	};
}

BOOST_AUTO_TEST_CASE(test_ion_reader_names)
{
	{
		TestData ion{"name1"};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 1, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 6, IonReader::Token::Type::End, ""));
	}
	{
		TestData ion{"    name1    "};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 5, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 14, IonReader::Token::Type::End, ""));
	}
	{
		TestData ion{" name1 name2 "};
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 2, IonReader::Token::Type::Name, "name1"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 8, IonReader::Token::Type::Name, "name2"));
		BOOST_CHECK_EQUAL(ion->read(), IonReader::Token(1, 14, IonReader::Token::Type::End, ""));
	}
}
