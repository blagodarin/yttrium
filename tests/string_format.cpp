#include <yttrium/date_time.h>
#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_string_format)
{
	DECLARE_MEMORY_MANAGER;

	BOOST_CHECK_EQUAL(String() << "", "");
	BOOST_CHECK_EQUAL(String() << "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	BOOST_CHECK_EQUAL(String() << "AB" << "CD" << "EF", "ABCDEF");

	BOOST_CHECK_EQUAL(String() << short{0}, "0");
	BOOST_CHECK_EQUAL(String() << static_cast<unsigned short>(0), "0");
	BOOST_CHECK_EQUAL(String() << 0, "0");
	BOOST_CHECK_EQUAL(String() << 0u, "0");
	BOOST_CHECK_EQUAL(String() << 0l, "0");
	BOOST_CHECK_EQUAL(String() << 0ul, "0");
	BOOST_CHECK_EQUAL(String() << 0ll, "0");
	BOOST_CHECK_EQUAL(String() << 0ull, "0");

	BOOST_CHECK_EQUAL(String() << 'A', "A");
	BOOST_CHECK_EQUAL(String() << static_cast<signed char>('A'), "65");
	BOOST_CHECK_EQUAL(String() << static_cast<unsigned char>('A'), "65");

	BOOST_CHECK_EQUAL(String() << 0.0, "0");
	BOOST_CHECK_EQUAL(String() << 7.25f, "7.25");
	BOOST_CHECK_EQUAL(String() << 3.625, "3.625");

	BOOST_CHECK_EQUAL(String() << rep('A', 0), "");
	BOOST_CHECK_EQUAL(String() << rep('A', 1), "A");
	BOOST_CHECK_EQUAL(String() << rep('A', 2), "AA");
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

	BOOST_CHECK_EQUAL(String() << print(date_time, "%Y-%M-%D %h:%m:%s.%z"), "2000-1-2 3:4:5.6");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%YY-%MM-%DD %hh:%mm:%ss.%zz"), "2000-01-02 03:04:05.006");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%YYY-%MMM-%DDD %hhh:%mmm:%sss.%zzz"), "2000Y-01M-02D 03h:04m:05s.006z");

	BOOST_CHECK_EQUAL(String() << print(date_time, "%"), "%");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%%"), "%");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%%%"), "%%");

	BOOST_CHECK_EQUAL(String() << print(date_time, "%A%B%C%_%E%F%G%H%I%J%K%L%_"), "%A%B%C%_%E%F%G%H%I%J%K%L%_");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%N%O%P%Q%R%S%T%U%V%W%X%_%Z"), "%N%O%P%Q%R%S%T%U%V%W%X%_%Z");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%a%b%c%d%e%f%g%_%i%j%k%l%_"), "%a%b%c%d%e%f%g%_%i%j%k%l%_");
	BOOST_CHECK_EQUAL(String() << print(date_time, "%n%o%p%q%r%_%t%u%v%w%x%y%_"), "%n%o%p%q%r%_%t%u%v%w%x%y%_");
}
