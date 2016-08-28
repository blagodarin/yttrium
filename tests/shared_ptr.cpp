#include <yttrium/memory/shared_ptr.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

class Counter : public Shared
{
public:
	Counter(int& counter) : _counter(counter) { ++_counter; }
	~Counter() override { --_counter; }
private:
	int& _counter;
};

BOOST_AUTO_TEST_CASE(test_shared_ptr)
{
	int counter = 0;
	{
		const auto& p1 = make_shared<Counter>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_shared_ptr_move)
{
	int counter = 0;

	auto&& p1 = make_shared<Counter>(*DefaultAllocator, counter);
	BOOST_CHECK_EQUAL(counter, 1);
	{
		const auto p2 = std::move(p1);
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_shared_ptr_copy)
{
	int counter = 0;
	{
		auto&& p1 = make_shared<Counter>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);

		const auto p2 = p1;
		BOOST_CHECK_EQUAL(counter, 1);

		p1 = {};
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_shared_ptr_from_unique_ptr)
{
	int counter = 0;
	{
		auto&& p1 = make_unique<Counter>(*DefaultAllocator, counter);
		BOOST_CHECK_EQUAL(counter, 1);

		SharedPtr<Counter> p2(std::move(p1));
		BOOST_CHECK_EQUAL(counter, 1);
	}
	BOOST_CHECK_EQUAL(counter, 0);
}
