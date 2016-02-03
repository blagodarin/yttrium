#include <yttrium/types.h>
#include "src/base/private_base.h"

#include <boost/test/unit_test.hpp>

class Public
{
public:

	class Private: public Yttrium::PrivateBase<Private>
	{
	public:

		Private(Yttrium::Allocator* allocator): PrivateBase(allocator) {}
	};

	void open()
	{
		if (!_private)
			_private = Y_NEW(Yttrium::DefaultAllocator, Private)(Yttrium::DefaultAllocator);
	}

	Private* get() const
	{
		return _private;
	}

private:
	Y_SHARED_PRIVATE(Public);
};

Y_IMPLEMENT_SHARED(Public);

BOOST_AUTO_TEST_CASE(test_private)
{
	Public public1;

	BOOST_CHECK(!public1.get());

	public1.open();

	BOOST_REQUIRE(public1.get());
	BOOST_CHECK_EQUAL(public1.get()->_references, 1);

	Public public2(public1);

	BOOST_CHECK_EQUAL(public2.get(), public1.get());
	BOOST_CHECK_EQUAL(public1.get()->_references, 2);

	{
		Public public3;

		public3.open();

		BOOST_CHECK(public3.get() != public1.get());

		Public public4(public3);

		BOOST_CHECK_EQUAL(public4.get(), public3.get());
		BOOST_CHECK_EQUAL(public3.get()->_references, 2);

		public3 = public1;

		BOOST_CHECK_EQUAL(public3.get(), public1.get());
		BOOST_CHECK_EQUAL(public1.get()->_references, 3);
		BOOST_CHECK_EQUAL(public4.get()->_references, 1);
	}

	BOOST_CHECK_EQUAL(public1.get()->_references, 2);

	Public public3(public2);

	BOOST_CHECK_EQUAL(public1.get()->_references, 3);

	public3 = public1;

	BOOST_CHECK_EQUAL(public1.get()->_references, 3);

	public3 = Public();

	BOOST_CHECK_EQUAL(public1.get()->_references, 2);

	public2 = Public();

	BOOST_CHECK_EQUAL(public1.get()->_references, 1);

	public1 = public1;

	BOOST_CHECK_EQUAL(public1.get()->_references, 1);

	public1 = Public();

	BOOST_CHECK(!public1.get());
}
