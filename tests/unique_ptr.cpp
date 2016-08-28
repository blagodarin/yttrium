#include <yttrium/memory/unique_ptr.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

class Counter
{
public:
	Counter(int& counter) : _counter(counter) { ++_counter; }
	~Counter() { --_counter; }
private:
	int& _counter;
};

BOOST_AUTO_TEST_CASE(test_unique_ptr)
{
	int counter = 0;
	{
		const auto& p1 = make_unique<Counter>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_unique_ptr_move)
{
	int counter = 0;

	auto&& p1 = make_unique<Counter>(*DefaultAllocator, counter);
	BOOST_CHECK_EQUAL(counter, 1);
	{
		const auto p2 = std::move(p1);
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}
