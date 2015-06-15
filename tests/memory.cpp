#include <yttrium/memory_manager.h>
#include <yttrium/proxy_allocator.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_default_allocator)
{
	DECLARE_MEMORY_MANAGER;

	Allocator *allocator = MemoryManager::default_allocator();

	void *p = allocator->allocate(1);

	BOOST_REQUIRE(p);

	p = allocator->reallocate(p, 2);

	BOOST_REQUIRE(p);

	allocator->deallocate(p);
}

BOOST_AUTO_TEST_CASE(test_default_allocator_difference)
{
	DECLARE_MEMORY_MANAGER;

	Allocator *allocator = MemoryManager::default_allocator();

	Allocator::Difference d;

	void *p = allocator->allocate(1, 0, &d);

	BOOST_REQUIRE(p);
//	BOOST_CHECK_GE(d.allocated, 1);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, 3, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
//	BOOST_CHECK_GE(d.allocated, 2);
//	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Increment);

	p = allocator->reallocate(p, 2, Allocator::MayMove, &d);

	BOOST_REQUIRE(p);
//	BOOST_CHECK_GE(d.allocated, 2);
//	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);

	allocator->deallocate(p, &d);

//	BOOST_CHECK_GE(d.allocated, 2);
	BOOST_CHECK_EQUAL(d.direction, Allocator::Difference::Decrement);
}
