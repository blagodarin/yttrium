#include <Yttrium/memory_manager.h>

#include "private.h"

#define BOOST_TEST_MODULE private

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, Yttrium::MemoryManager)

BOOST_AUTO_TEST_CASE(private_test)
{
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

BOOST_AUTO_TEST_SUITE_END()
