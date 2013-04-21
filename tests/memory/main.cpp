#include <Yttrium/memory_manager.h>
#include <Yttrium/proxy_allocator.h>

#define BOOST_TEST_MODULE memory

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(default_allocator_test)
{
	Allocator *allocator = MemoryManager::default_allocator();

	void *p = allocator->allocate(1);

	BOOST_REQUIRE(p);

	p = allocator->reallocate(p, 2);

	BOOST_REQUIRE(p);

	allocator->deallocate(p);
}

BOOST_AUTO_TEST_CASE(default_difference_test)
{
	Allocator *allocator = MemoryManager::default_allocator();

	Allocator::Difference d;

	void *p = allocator->allocate(1, 0, &d);

	BOOST_REQUIRE(p);
//	BOOST_CHECK(d.allocated >= 1);
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, 3, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK(d.total >= d.allocated);

	p = allocator->reallocate(p, 2, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK(d.total >= d.allocated);

	allocator->deallocate(p, &d);

//	BOOST_CHECK(d.allocated >= 2);
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(system_allocator_test)
{
	SystemAllocator *allocator = SystemAllocator::instance();

	void *p = allocator->allocate(1);

	BOOST_REQUIRE(p);

	p = allocator->reallocate(p, 2);

	BOOST_REQUIRE(p);

	allocator->deallocate(p);
}

BOOST_AUTO_TEST_CASE(system_difference_test)
{
	SystemAllocator *allocator = SystemAllocator::instance();

	size_t small = 1;
	size_t medium = allocator->upper_bound(small) + 1;
	size_t big = allocator->upper_bound(medium) + 1;

	BOOST_CHECK(medium > small);
	BOOST_CHECK(big > medium);

	Allocator::Difference d;

	void *p = allocator->allocate(small, 0, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK(d.allocated == allocator->upper_bound(small));
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, big, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_EQUAL(d.allocated, allocator->upper_bound(big) - allocator->upper_bound(small));
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, medium, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_EQUAL(d.allocated, allocator->upper_bound(big) - allocator->upper_bound(medium));
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);

	allocator->deallocate(p, &d);

	BOOST_CHECK_EQUAL(d.allocated, allocator->upper_bound(medium));
	BOOST_CHECK(d.total >= d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);
}
