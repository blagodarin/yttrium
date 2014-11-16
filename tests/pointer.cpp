#include <yttrium/allocator.h>
#include <yttrium/pointer.h>

#include "common.h"

class Pointable: public Yttrium::Pointable
{
public:

	Pointable(int& counter, Yttrium::Allocator* allocator)
		: Yttrium::Pointable(allocator)
		, _counter(counter)
	{
		++_counter;
	}

	~Pointable() override
	{
		--_counter;
	}

private:

	int& _counter;
};

typedef Yttrium::Pointer<Pointable> Pointer;

BOOST_AUTO_TEST_CASE(test_pointer)
{
	DECLARE_MEMORY_MANAGER;

	Yttrium::Allocator* allocator = Yttrium::DefaultAllocator;

	int counter = 0;

	{
		Pointer p1(Y_NEW(allocator, Pointable)(counter, allocator));
		BOOST_CHECK_EQUAL(counter, 1);

		{
			Pointer p2(Y_NEW(allocator, Pointable)(counter, allocator));
			BOOST_CHECK_EQUAL(counter, 2);

			Pointer p3 = p2;
			BOOST_CHECK_EQUAL(counter, 2);
		}

		BOOST_CHECK_EQUAL(counter, 1);
	}

	BOOST_CHECK_EQUAL(counter, 0);
}
