#include "src/base/private_base.h"

#include "common.h"

struct Public
{
	struct Private: public Yttrium::PrivateBase<Private>
	{
		Private(Yttrium::Allocator* allocator): PrivateBase(allocator) {}
	};

	Private* _private;

	Public(): _private(nullptr) {}

	Public(const Public& public_): _private(Private::copy(public_._private)) {}

	~Public()
	{
		close();
	}

	void close()
	{
		Private::release(&_private);
	}

	void open()
	{
		if (!_private)
			_private = Y_NEW(Yttrium::DefaultAllocator, Private)(Yttrium::DefaultAllocator);
	}

	Public& operator =(const Public& public_)
	{
		Private::assign(&_private, public_._private);
		return *this;
	}
};

BOOST_AUTO_TEST_CASE(private_test)
{
	DECLARE_MEMORY_MANAGER;

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
