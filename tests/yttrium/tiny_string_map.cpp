#include <yttrium/utils/tiny_string_map.h>
#include "iostream.h"

#include <catch2/catch.hpp>

using Yttrium::TinyStringMap;

TEST_CASE("tiny_string_map.find")
{
	TinyStringMap m;
	CHECK(!m.find({}).data());
	CHECK(!m.find("test").data());
}

TEST_CASE("tiny_string_map.insert")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	CHECK(m.find({}) == "");
	CHECK(m.find({}).data());
	CHECK(!m.find("test").data());
	m.insert_or_assign("test", "value");
	CHECK(m.find("test") == "value");
	CHECK(m.find({}) == "");
	CHECK(!m.find("another").data());
	m.insert_or_assign("another", "test value");
	CHECK(m.find("another") == "test value");
	CHECK(m.find("test") == "value");
	CHECK(m.find({}) == "");
}

TEST_CASE("tiny_string_map.erase")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	REQUIRE(m.find({}) == "");
	m.erase({});
	CHECK(!m.find({}).data());
	CHECK(m.find("test") == "value");
	CHECK(m.find("another") == "test value");
	m.erase("test");
	CHECK(!m.find({}).data());
	CHECK(!m.find("test").data());
	CHECK(m.find("another") == "test value");
	m.erase("another");
	CHECK(!m.find({}).data());
	CHECK(!m.find("test").data());
	CHECK(!m.find("another").data());
}

TEST_CASE("tiny_string_map.assign")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	REQUIRE(m.find("test") == "value");
	m.insert_or_assign("test", "longer value");
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "longer value");
	CHECK(m.find("another") == "test value");
	m.insert_or_assign("another", "short");
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "longer value");
	CHECK(m.find("another") == "short");
	m.insert_or_assign({}, "the longest value");
	CHECK(m.find({}) == "the longest value");
	CHECK(m.find("test") == "longer value");
	CHECK(m.find("another") == "short");
}

TEST_CASE("tiny_string_map.copy_construction")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	TinyStringMap n(m);
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "value");
	CHECK(m.find("another") == "test value");
	CHECK(n.find({}) == "");
	CHECK(n.find("test") == "value");
	CHECK(n.find("another") == "test value");
}

TEST_CASE("tiny_string_map.move_construction")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	TinyStringMap n(std::move(m));
	CHECK(n.find({}) == "");
	CHECK(n.find("test") == "value");
	CHECK(n.find("another") == "test value");
	CHECK(!m.find({}).data());
	CHECK(!m.find("test").data());
	CHECK(!m.find("another").data());
}

TEST_CASE("tiny_string_map.shrinking_assignment")
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
	CHECK(!m.find({}).data());
	CHECK(!m.find("test").data());
	CHECK(!m.find("another").data());
	CHECK(m.find("a") == "b");
	CHECK(m.find("c") == "d");
	CHECK(m.find("e") == "f");
	CHECK(n.find("a") == "b");
	CHECK(n.find("c") == "d");
	CHECK(n.find("e") == "f");
}

TEST_CASE("tiny_string_map.expanding_assignment")
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
	CHECK(!m.find("a").data());
	CHECK(!m.find("c").data());
	CHECK(!m.find("e").data());
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "value");
	CHECK(m.find("another") == "test value");
	CHECK(n.find({}) == "");
	CHECK(n.find("test") == "value");
	CHECK(n.find("another") == "test value");
}

TEST_CASE("tiny_string_map.self_assignment")
{
	TinyStringMap m;
	m.insert_or_assign({}, {});
	m.insert_or_assign("test", "value");
	m.insert_or_assign("another", "test value");
	m = m;
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "value");
	CHECK(m.find("another") == "test value");
}

TEST_CASE("tiny_string_map.move_assignment")
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
	CHECK(!m.find("a").data());
	CHECK(!m.find("c").data());
	CHECK(!m.find("e").data());
	CHECK(m.find({}) == "");
	CHECK(m.find("test") == "value");
	CHECK(m.find("another") == "test value");
	CHECK(!n.find({}).data());
	CHECK(!n.find("test").data());
	CHECK(!n.find("another").data());
}

TEST_CASE("tiny_string_map.limit")
{
	TinyStringMap m;
	const std::string tiny(std::numeric_limits<uint8_t>::max(), 'a');
	const std::string not_tiny(tiny.size() + 1, 'a');
	CHECK_THROWS_AS(m.insert_or_assign(not_tiny, not_tiny), std::length_error);
	CHECK_THROWS_AS(m.insert_or_assign(not_tiny, tiny), std::length_error);
	CHECK_THROWS_AS(m.insert_or_assign(tiny, not_tiny), std::length_error);
	CHECK_NOTHROW(m.insert_or_assign(tiny, tiny));
	CHECK_THROWS_AS(m.insert_or_assign(tiny, not_tiny), std::length_error); // Throws before key lookup.
}
