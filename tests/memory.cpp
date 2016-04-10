#include <yttrium/memory/allocator.h>
#include <yttrium/memory/global.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_default_allocator)
{
	auto p = DefaultAllocator->allocate(1);

	BOOST_CHECK(p);

	DefaultAllocator->deallocate(p);

	BOOST_CHECK(p);
}
