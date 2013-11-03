#include <yttrium/allocator.h>
#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE(pointer_test)
{
	Yttrium::MemoryManager memory_manager;

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
