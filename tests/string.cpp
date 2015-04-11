#include <yttrium/string.h>
#include <yttrium/time.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_string_initialization)
{
	String s1;

	BOOST_CHECK_EQUAL(s1.size(), 0);
	BOOST_CHECK(s1.is_empty());
	BOOST_CHECK(s1.text());
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 0);

	String s2;

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(test_string_assignment)
{
	DECLARE_MEMORY_MANAGER;

	String s1 = "test";

	BOOST_CHECK_EQUAL(s1.size(), 4);
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 't');
	BOOST_CHECK_EQUAL(s1[2], 's');
	BOOST_CHECK(s1 == "test");

	String s2 = s1;

	BOOST_CHECK(s1 == s2);

	String s3 = s2;

	s2 = "another test";

	BOOST_CHECK(s1 == "test");
	BOOST_CHECK(s2 == "another test");
	BOOST_CHECK(s3 == s1);

	s1 += "!";

	BOOST_CHECK(s1 == "test!");
	BOOST_CHECK(s3 == "test");
}

BOOST_AUTO_TEST_CASE(test_string_swap)
{
	DECLARE_MEMORY_MANAGER;

	String s1 = "test";
	String s2 = "another test";

	s2.swap(&s1);

	BOOST_CHECK(s1 == "another test");
	BOOST_CHECK(s2 == "test");

	s1.swap("rvalue test");

	BOOST_CHECK(s1 == "rvalue test");
}

BOOST_AUTO_TEST_CASE(test_string_insert)
{
	DECLARE_MEMORY_MANAGER;

	String s1 = "test";

	s1.insert("just ", 0);

	BOOST_CHECK(s1 == "just test");

	s1.insert("another ", 5);

	BOOST_CHECK(s1 == "just another test");

	s1.insert('!', s1.size());

	BOOST_CHECK(s1 == "just another test!");

	s1.insert('!', s1.size() + 1);

	BOOST_CHECK(s1 == "just another test!");
}

BOOST_AUTO_TEST_CASE(test_string_format_date_time)
{
	DECLARE_MEMORY_MANAGER;

	DateTime date_time;
	date_time.year = 2000;
	date_time.month = 1;
	date_time.day = 2;
	date_time.hour = 3;
	date_time.minute = 4;
	date_time.second = 5;
	date_time.msecond = 6;

	BOOST_CHECK(String::format(date_time, "%Y-%M-%D %h:%m:%s.%z") == "2000-1-2 3:4:5.6");
	BOOST_CHECK(String::format(date_time, "%YY-%MM-%DD %hh:%mm:%ss.%zz") == "2000-01-02 03:04:05.006");
	BOOST_CHECK(String::format(date_time, "%YYY-%MMM-%DDD %hhh:%mmm:%sss.%zzz") == "2000Y-01M-02D 03h:04m:05s.006z");

	BOOST_CHECK(String::format(date_time, "%") == "%");
	BOOST_CHECK(String::format(date_time, "%%") == "%");
	BOOST_CHECK(String::format(date_time, "%%%") == "%%");

	BOOST_CHECK(String::format(date_time, "%A%B%C%_%E%F%G%H%I%J%K%L%_") == "%A%B%C%_%E%F%G%H%I%J%K%L%_");
	BOOST_CHECK(String::format(date_time, "%N%O%P%Q%R%S%T%U%V%W%X%_%Z") == "%N%O%P%Q%R%S%T%U%V%W%X%_%Z");
	BOOST_CHECK(String::format(date_time, "%a%b%c%d%e%f%g%_%i%j%k%l%_") == "%a%b%c%d%e%f%g%_%i%j%k%l%_");
	BOOST_CHECK(String::format(date_time, "%n%o%p%q%r%_%t%u%v%w%x%y%_") == "%n%o%p%q%r%_%t%u%v%w%x%y%_");
}
