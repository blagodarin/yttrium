#include <Yttrium/memory_manager.h>
#include <Yttrium/string.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(string_initialization_test)
{
	String s1;

	BOOST_CHECK_EQUAL(s1.size(), 0);
	BOOST_CHECK(s1.is_empty());
	BOOST_CHECK(s1.text());
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 0);

	String s2;

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(string_assignment_test)
{
	MemoryManager memory_manager;

	String s1 = "test";

	BOOST_CHECK_EQUAL(s1.size(), 4);
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 't');
	BOOST_CHECK_EQUAL(s1[2], 's');
	BOOST_CHECK(s1 == "test");

	String s2 = s1;

	BOOST_CHECK(s1 == s2);

	String s3 = s2;

	s2 = "another test";

	BOOST_CHECK(s1 == "test");
	BOOST_CHECK(s2 == "another test");
	BOOST_CHECK(s3 == s1);

	s1 += "!";

	BOOST_CHECK(s1 == "test!");
	BOOST_CHECK(s3 == "test");
}

BOOST_AUTO_TEST_CASE(string_swap_test)
{
	MemoryManager memory_manager;

	String s1 = "test";
	String s2 = "another test";

	s2.swap(&s1);

	BOOST_CHECK(s1 == "another test");
	BOOST_CHECK(s2 == "test");

	s1.swap("rvalue test");

	BOOST_CHECK(s1 == "rvalue test");
}

BOOST_AUTO_TEST_CASE(string_insert_test)
{
	MemoryManager memory_manager;

	String s1 = "test";

	s1.insert("just ", 0);

	BOOST_CHECK(s1 == "just test");

	s1.insert("another ", 5);

	BOOST_CHECK(s1 == "just another test");

	s1.insert('!', s1.size());

	BOOST_CHECK(s1 == "just another test!");

	s1.insert('!', s1.size() + 1);

	BOOST_CHECK(s1 == "just another test!");
}