#include <yttrium/tiny_string_map.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::TinyStringMap;

BOOST_AUTO_TEST_CASE(test_tiny_string_map_find)
{
	TinyStringMap m;
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_insert)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK(m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
	m.insert_or_assign("test", "value");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK(!m.find("another").data());
	m.insert_or_assign("another", "test value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find({}), "");
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_erase)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	BOOST_REQUIRE_EQUAL(m.find({}), "");
	m.erase({});
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	m.erase("test");
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	m.erase("another");
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
	BOOST_CHECK(!m.find("another").data());
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_assign)
{
	TinyStringMap m;
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

BOOST_AUTO_TEST_CASE(test_tiny_string_map_copy_construction)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	TinyStringMap n(m);
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	BOOST_CHECK_EQUAL(n.find({}), "");
	BOOST_CHECK_EQUAL(n.find("test"), "value");
	BOOST_CHECK_EQUAL(n.find("another"), "test value");
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_move_construction)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	TinyStringMap n(std::move(m));
	BOOST_CHECK_EQUAL(n.find({}), "");
	BOOST_CHECK_EQUAL(n.find("test"), "value");
	BOOST_CHECK_EQUAL(n.find("another"), "test value");
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
	BOOST_CHECK(!m.find("another").data());
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_shrinking_assignment)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	TinyStringMap n;
	n.insert_or_assign("a", "b");
	n.insert_or_assign("c", "d");
	n.insert_or_assign("e", "f");
	m = n;
	BOOST_CHECK(!m.find({}).data());
	BOOST_CHECK(!m.find("test").data());
	BOOST_CHECK(!m.find("another").data());
	BOOST_CHECK_EQUAL(m.find("a"), "b");
	BOOST_CHECK_EQUAL(m.find("c"), "d");
	BOOST_CHECK_EQUAL(m.find("e"), "f");
	BOOST_CHECK_EQUAL(n.find("a"), "b");
	BOOST_CHECK_EQUAL(n.find("c"), "d");
	BOOST_CHECK_EQUAL(n.find("e"), "f");
}

BOOST_AUTO_TEST_CASE(test_tiny_string_expanding_assignment)
{
	TinyStringMap m;
	m.insert_or_assign("a", "b");
	m.insert_or_assign("c", "d");
	m.insert_or_assign("e", "f");
	TinyStringMap n;
	n.insert_or_assign({}, {});
	n.insert_or_assign("test", "value");
	n.insert_or_assign("another", "test value");
	m = n;
	BOOST_CHECK(!m.find("a").data());
	BOOST_CHECK(!m.find("c").data());
	BOOST_CHECK(!m.find("e").data());
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	BOOST_CHECK_EQUAL(n.find({}), "");
	BOOST_CHECK_EQUAL(n.find("test"), "value");
	BOOST_CHECK_EQUAL(n.find("another"), "test value");
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_self_assignment)
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	m = m;
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_move_assignment)
{
	TinyStringMap m;
	m.insert_or_assign("a", "b");
	m.insert_or_assign("c", "d");
	m.insert_or_assign("e", "f");
	TinyStringMap n;
	n.insert_or_assign({}, {});
	n.insert_or_assign("test", "value");
	n.insert_or_assign("another", "test value");
	m = std::move(n);
	BOOST_CHECK(!m.find("a").data());
	BOOST_CHECK(!m.find("c").data());
	BOOST_CHECK(!m.find("e").data());
	BOOST_CHECK_EQUAL(m.find({}), "");
	BOOST_CHECK_EQUAL(m.find("test"), "value");
	BOOST_CHECK_EQUAL(m.find("another"), "test value");
	BOOST_CHECK(!n.find({}).data());
	BOOST_CHECK(!n.find("test").data());
	BOOST_CHECK(!n.find("another").data());
}

BOOST_AUTO_TEST_CASE(test_tiny_string_map_limit)
{
	TinyStringMap m;
	const std::string tiny(std::numeric_limits<uint8_t>::max(), 'a');
	const std::string not_tiny(tiny.size() + 1, 'a');
	BOOST_CHECK_THROW(m.insert_or_assign(not_tiny, not_tiny), std::length_error);
	BOOST_CHECK_THROW(m.insert_or_assign(not_tiny, tiny), std::length_error);
	BOOST_CHECK_THROW(m.insert_or_assign(tiny, not_tiny), std::length_error);
	BOOST_CHECK_NO_THROW(m.insert_or_assign(tiny, tiny));
	BOOST_CHECK_THROW(m.insert_or_assign(tiny, not_tiny), std::length_error); // Throws before key lookup.
}
