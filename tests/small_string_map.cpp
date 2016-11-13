#include <yttrium/small_string_map.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_small_string_map_find)
{
	SmallStringMap m;
	BOOST_CHECK(m.find({}).is_null());
	BOOST_CHECK(m.find("test").is_null());
}

BOOST_AUTO_TEST_CASE(test_small_string_map_insert)
{
	SmallStringMap m;
	m.insert_or_assign({}, {});
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK(!m.find({}).is_null());
	BOOST_CHECK(m.find("test").is_null());
	m.insert_or_assign("test", "value");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK(m.find("another").is_null());
	m.insert_or_assign("another", "test value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find({}), "");
}

BOOST_AUTO_TEST_CASE(test_small_string_map_erase)
{
	SmallStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	BOOST_REQUIRE_EQUAL(m.find({}), "");
	m.erase({});
	BOOST_CHECK(m.find({}).is_null());
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	m.erase("test");
	BOOST_CHECK(m.find({}).is_null());
	BOOST_CHECK(m.find("test").is_null());
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	m.erase("another");
	BOOST_CHECK(m.find({}).is_null());
	BOOST_CHECK(m.find("test").is_null());
	BOOST_CHECK(m.find("another").is_null());
}

BOOST_AUTO_TEST_CASE(test_small_string_map_assign)
{
	SmallStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	BOOST_REQUIRE_EQUAL(m.find("test"), "value");
	m.insert_or_assign("test", "longer value");
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "longer value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	m.insert_or_assign("another", "short");
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "longer value");
	BOOST_CHECK_EQUAL(m.find("another"), "short");
	m.insert_or_assign({}, "the longest value");
	BOOST_CHECK_EQUAL(m.find({}), "the longest value");
	BOOST_CHECK_EQUAL(m.find("test"), "longer value");
	BOOST_CHECK_EQUAL(m.find("another"), "short");
}
