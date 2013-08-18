#include <yttrium/memory_manager.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/system_allocator.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(default_allocator_test)
{
	MemoryManager memory_manager;

	Allocator *allocator = MemoryManager::default_allocator();

	void *p = allocator->allocate(1);

	BOOST_REQUIRE(p);

	p = allocator->reallocate(p, 2);

	BOOST_REQUIRE(p);

	allocator->deallocate(p);
}

BOOST_AUTO_TEST_CASE(default_allocator_difference_test)
{
	MemoryManager memory_manager;

	Allocator *allocator = MemoryManager::default_allocator();

	Allocator::Difference d;

	void *p = allocator->allocate(1, 0, &d);

	BOOST_REQUIRE(p);
//	BOOST_CHECK_GE(d.allocated, 1);
	BOOST_CHECK_GE(d.total, d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, 3, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_GE(d.total, d.allocated);

	p = allocator->reallocate(p, 2, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_GE(d.total, d.allocated);

	allocator->deallocate(p, &d);

//	BOOST_CHECK_GE(d.allocated, 2);
	BOOST_CHECK_GE(d.total, d.allocated);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);
}

BOOST_AUTO_TEST_CASE(system_allocator_test)
{
	SystemAllocator *allocator = SystemAllocator::instance();

	void *p = allocator->allocate(1);

	BOOST_REQUIRE(p);

	p = allocator->reallocate(p, 2);

	BOOST_REQUIRE(p);

	allocator->deallocate(p);
}

BOOST_AUTO_TEST_CASE(system_allocator_difference_test)
{
	SystemAllocator *allocator = SystemAllocator::instance();

	size_t small = 1;
	size_t medium = allocator->upper_bound(small) + 1;
	size_t big = allocator->upper_bound(medium) + 1;

	size_t page_size = big - medium;
	size_t reserved_size = page_size - allocator->upper_bound(0);

	BOOST_CHECK_GT(medium, small);
	BOOST_CHECK_GT(big, medium);

	Allocator::Difference d;

	void *p = allocator->allocate(small, 0, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_EQUAL(d.allocated, small);
	BOOST_CHECK_GE(d.total, allocator->upper_bound(small) + reserved_size);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, big, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_EQUAL(d.allocated, big - small);
	BOOST_CHECK_GE(d.total, allocator->upper_bound(big) - allocator->upper_bound(small));
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, medium, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
	BOOST_CHECK_EQUAL(d.allocated, big - medium);
	BOOST_CHECK_GE(d.total, allocator->upper_bound(big) - allocator->upper_bound(medium));
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);

	allocator->deallocate(p, &d);

	BOOST_CHECK_EQUAL(d.allocated, medium);
	BOOST_CHECK_GE(d.total, allocator->upper_bound(medium) + reserved_size);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);
}
