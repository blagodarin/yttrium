#include <yttrium/memory/global.h>
#include <yttrium/memory/pointer.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_pointer)
{
	struct Object
	{
		int& _counter;

		Object(int& counter) : _counter(counter) { ++_counter; }
		~Object() { --_counter; }
	};

	int counter = 0;
	{
		const auto p1 = make_pointer<Object>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);
		{
			auto p2 = make_pointer<Object>(*DefaultAllocator, counter);
			BOOST_CHECK_EQUAL(counter, 2);
			auto p3 = std::move(p2);
			BOOST_CHECK(!p2);
			BOOST_CHECK_EQUAL(counter, 2);
		}
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}
