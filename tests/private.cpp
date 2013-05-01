#include <Yttrium/memory_manager.h>

#include "src/base/private_base.h"

#include <boost/test/unit_test.hpp>

class Public
{
public:

	class Private
		: public Yttrium::PrivateBase<Private>
	{
	public:

		Private(Yttrium::Allocator *allocator)
			: PrivateBase(allocator)
		{
		}
	};

public:

	Public()
		: _private(nullptr)
	{
	}

	Public(const Public &public_)
		: _private(Private::copy(public_._private))
	{
	}

	~Public()
	{
		close();
	}

public:

	void close()
	{
		Private::release(&_private);
	}

	void open(Yttrium::Allocator *allocator = Yttrium::DefaultAllocator)
	{
		if (!_private)
		{
			_private = Y_NEW(allocator, Private)(allocator);
		}
	}

public:

	Public &operator =(const Public &public_)
	{
		Private::assign(&_private, public_._private);
		return *this;
	}

public:

	Private *_private;
};

BOOST_AUTO_TEST_CASE(private_test)
{
	Yttrium::MemoryManager memory_manager;

	Public public1;

	BOOST_CHECK(!public1._private);

	public1.open();

	BOOST_REQUIRE(public1._private);
	BOOST_CHECK_EQUAL(public1._private->_references, 1);

	Public public2(public1);

	BOOST_CHECK_EQUAL(public2._private, public1._private);
	BOOST_CHECK_EQUAL(public1._private->_references, 2);

	{
		Public public3;

		public3.open();

		BOOST_CHECK(public3._private != public1._private);

		Public public4(public3);

		BOOST_CHECK_EQUAL(public4._private, public3._private);
		BOOST_CHECK_EQUAL(public3._private->_references, 2);

		public3 = public1;

		BOOST_CHECK_EQUAL(public3._private, public1._private);
		BOOST_CHECK_EQUAL(public1._private->_references, 3);
		BOOST_CHECK_EQUAL(public4._private->_references, 1);
	}

	BOOST_CHECK_EQUAL(public1._private->_references, 2);

	Public public3(public2);

	BOOST_CHECK_EQUAL(public1._private->_references, 3);

	public3 = public1;

	BOOST_CHECK_EQUAL(public1._private->_references, 3);

	public3.close();

	BOOST_CHECK_EQUAL(public1._private->_references, 2);

	public2.close();

	BOOST_CHECK_EQUAL(public1._private->_references, 1);

	public1 = public1;

	BOOST_CHECK_EQUAL(public1._private->_references, 1);

	public1.close();

	BOOST_CHECK(!public1._private);
}
