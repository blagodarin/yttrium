#include <yttrium/memory/shared_ptr.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_shared_ptr)
{
	class Counter : public Object
	{
	public:
		Counter(int& counter) : _counter(counter) { ++_counter; }
		~Counter() override { --_counter; }
	private:
		int& _counter;
	};

	int counter = 0;
	{
		const auto p1 = make_shared<Counter>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);
		{
			auto p2 = make_shared<Counter>(*DefaultAllocator, counter);
			BOOST_CHECK_EQUAL(counter, 2);
			const auto p3 = p2;
			BOOST_CHECK(p2);
			BOOST_CHECK_EQUAL(counter, 2);
			p2 = p1;
			BOOST_CHECK_EQUAL(counter, 2);
			p2 = make_shared<Counter>(*DefaultAllocator, counter);
			BOOST_CHECK_EQUAL(counter, 3);
		}
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}
