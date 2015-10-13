#include <yttrium/allocator.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_default_allocator)
{
	DECLARE_MEMORY_MANAGER;

	const auto allocator = MemoryManager::default_allocator();

	auto p = allocator->allocate(1);

	BOOST_CHECK(p);

	allocator->deallocate(p);

	BOOST_CHECK(p);
}
