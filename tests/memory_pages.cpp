#include <yttrium/memory/pages.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_page_allocation)
{
	{
		PageAllocation allocation(1);
		BOOST_CHECK(allocation.size() >= 1);
	}
	{
		PageAllocation allocation(1000000);
		BOOST_CHECK(allocation.size() >= 1000000);
	}
}
