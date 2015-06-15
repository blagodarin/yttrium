#include <yttrium/object.h>

#include "common.h"

class Object : public Yttrium::Object
{
public:

	Object(int& counter, Yttrium::Allocator* allocator)
		: Yttrium::Object(allocator)
		, _counter(counter)
	{
		++_counter;
	}

	~Object() override
	{
		--_counter;
	}

private:

	int& _counter;
};

BOOST_AUTO_TEST_CASE(test_unique_ptr)
{
	using UniquePtr = Yttrium::UniquePtr<Object>;

	DECLARE_MEMORY_MANAGER;

	Yttrium::Allocator* allocator = Yttrium::DefaultAllocator;

	int counter = 0;

	{
		UniquePtr p1(Y_NEW(allocator, Object)(counter, allocator));
		BOOST_CHECK_EQUAL(counter, 1);

		{
			UniquePtr p2(Y_NEW(allocator, Object)(counter, allocator));
			BOOST_CHECK_EQUAL(counter, 2);

			UniquePtr p3 = std::move(p2);
			BOOST_CHECK_EQUAL(counter, 2);
		}

		BOOST_CHECK_EQUAL(counter, 1);
	}

	BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_shared_ptr)
{
	using SharedPtr = Yttrium::SharedPtr<Object>;

	DECLARE_MEMORY_MANAGER;

	Yttrium::Allocator* allocator = Yttrium::DefaultAllocator;

	int counter = 0;

	{
		SharedPtr p1(Y_NEW(allocator, Object)(counter, allocator));
		BOOST_CHECK_EQUAL(counter, 1);

		{
			SharedPtr p2(Y_NEW(allocator, Object)(counter, allocator));
			BOOST_CHECK_EQUAL(counter, 2);

			SharedPtr p3 = p2;
			BOOST_CHECK_EQUAL(counter, 2);
		}

		BOOST_CHECK_EQUAL(counter, 1);
	}

	BOOST_CHECK_EQUAL(counter, 0);
}
