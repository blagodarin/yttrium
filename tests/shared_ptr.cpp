#include <yttrium/memory/global.h>
#include <yttrium/memory/shared_ptr.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_shared_ptr)
{
	class Derived : public Object
	{
	public:
		Derived(int& counter, Allocator* allocator) : Object(allocator), _counter(counter) { ++_counter; }
		~Derived() override { --_counter; }
	private:
		int& _counter;
	};

	int counter = 0;
	{
		auto p1 = make_shared<Derived>(*DefaultAllocator, counter, DefaultAllocator);
		BOOST_CHECK_EQUAL(counter, 1);
		{
			auto p2 = make_shared<Derived>(*DefaultAllocator, counter, DefaultAllocator);
			BOOST_CHECK_EQUAL(counter, 2);
			auto p3 = p2;
			BOOST_CHECK(p2);
			BOOST_CHECK_EQUAL(counter, 2);
		}
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}
